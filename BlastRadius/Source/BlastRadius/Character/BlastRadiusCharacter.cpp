// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "BlastRadiusCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Character/CharacterAnimInstance.h"
#include "Component/BlinkComponent.h"
#include "Component/HealthComponent.h"
#include "Component/EnergyComponent.h"
#include "Weapon/BlastRadiusSword.h"
#include "Weapon/BlastRadiusProjectile.h"
#include "Weapon/BlastRadiusGrenade.h"
#include "Pickup/BlastRadiusPickup.h"
#include "Pickup/BlastRadiusBattery.h"
#include "Gameplay/BlastRadiusGameStateBase.h"
#include "Gameplay/BlastRadiusPlayerState.h"
#include "Weapon/BlastRadiusWeapon.h"
#include "Gameplay/BlastRadiusGameMode.h"
#include "Gameplay/BlastRadiusPlayerController.h"
#include "Runtime/Engine/Classes/Engine/CollisionProfile.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"

//////////////////////////////////////////////////////////////////////////
// ABlastRadiusCharacter

ABlastRadiusCharacter::ABlastRadiusCharacter() :
    SkeletalMesh(nullptr),
    BlinkComponent(nullptr),
    HealthComponent(nullptr),
    EnergyComponent(nullptr),
    AnimationInstance(nullptr)
{
    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
    GetCapsuleComponent()->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
    GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);

    // Don't rotate when the controller rotates. Let that just affect the camera.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    GetCharacterMovement()->MaxWalkSpeed = MaxRunSpeed;
    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
    GetCharacterMovement()->SetJumpAllowed(false);

    // Create a camera boom (pulls in towards the player if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->bEnableCameraLag = true;
    CameraBoom->CameraLagSpeed = 50.0f;
    CameraBoom->CameraLagMaxDistance = 250.0f;
    CameraBoom->bDoCollisionTest = false;

    // Create a follow camera
    TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    TopDownCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
    TopDownCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

    // Create static mesh for the helmet
    HelmetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Helmet Mesh"));
    HelmetMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Create audio component
    AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Component"));
    AudioComponent->SetupAttachment(RootComponent);

    // Setup the health and energy components
    HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
    EnergyComponent = CreateDefaultSubobject<UEnergyComponent>(TEXT("Energy"));
    BlinkComponent = CreateDefaultSubobject<UBlinkComponent>(TEXT("Blink"));

    // Nullify variables.
    Sword = nullptr;
    Weapon = nullptr;

    // Set default spawn-delay.
    SpawnDelay = 1.0f;

    // Add unique tag.
    Tags.Add("Player");

    // Enable replication.
    SetReplicates(true);
    SetReplicateMovement(true);
}

void ABlastRadiusCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    /* Retrieve the skeletal mesh */
    SkeletalMesh = GetMesh();
    if (SkeletalMesh != nullptr)
    {
        check(SkeletalMesh != nullptr && "Character doesn't have a skeletal mesh!");
        //SkeletalMesh->SetCollisionObjectType(ECC_Pawn);

        /* Retrieve the animation instance */
        AnimationInstance = Cast<UCharacterAnimInstance>(SkeletalMesh->GetAnimInstance());
    }

    /* Attach Helmet Mesh to Skeletal Mesh */
    if (HelmetMesh)
    {
        check(HelmetMesh != nullptr && "Character doesn't have a helmet mesh!");

        HelmetMesh->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, "HelmetSocket");
    }

    /* Retrieve the health component */
    HealthComponent = FindComponentByClass<UHealthComponent>();

    //Check for health component
    if (HealthComponent != nullptr)
    {
        // Subscribe to death event
        HealthComponent->OnDeath.AddDynamic(this, &ABlastRadiusCharacter::NetMultiCastOnDeath);
    }

    // Setup overlap
    GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABlastRadiusCharacter::OnOverlapBegin);
}

class ABlastRadiusPlayerState* ABlastRadiusCharacter::GetPlayerState()
{
    return Cast<class ABlastRadiusPlayerState>(this->PlayerState);
}

class ABlastRadiusGameStateBase* ABlastRadiusCharacter::GetGameState()
{
    return Cast<ABlastRadiusGameStateBase>(GetWorld()->GetGameState());

}

void ABlastRadiusCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Spawn a projectile.
    FActorSpawnParameters SpawnParams;
    SpawnParams.Instigator = this;
    SpawnParams.Owner = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; //TILAN HERE AdjustIfPossibleButAlwaysSpawn

    if (SwordClass)
    {
        Sword = GetWorld()->SpawnActor<ABlastRadiusSword>(SwordClass, SpawnParams);
        Sword->Attach(this);
    }

    if (WeaponClass)
    {
        Weapon = GetWorld()->SpawnActor<ABlastRadiusWeapon>(WeaponClass, SpawnParams);
        Weapon->SetOwner(this);
        Weapon->Attach(this);
    }


    /* UpdateAndCheckPlayer at slower delay. */
    GetWorldTimerManager().SetTimer(UpdateHandle, this, &ABlastRadiusCharacter::UpdateAndCheckPlayer, 0.03333f, true, 0.0f);

    /* Prevent players from connecting at the same time. */
    GetWorldTimerManager().SetTimer(PostBeginPlayDelay, this, &ABlastRadiusCharacter::PostBeginPlay, 1.0f, false);

    if (Role > ROLE_AutonomousProxy)
    {
        // Assign net index a value of player number in gamemode.
        NetIndex = GetWorld()->GetGameState()->AuthorityGameMode->GetNumPlayers() - 1;
    }
}

void ABlastRadiusCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    /* Set movement state. */
    float CurrentSpeed = GetVelocity().Size(); // Get character's current speed
    bool bIsMoving = CurrentSpeed > 0.0f && GetCharacterMovement()->IsMovingOnGround(); // Check for character movement

    AnimationInstance->bIsMoving = bIsMoving;
    AnimationInstance->MovementSpeed = bIsMoving ? CurrentSpeed : 0.0f;

    /* Set animation strafing rotation parameter.*/
    FVector MovementDirection = GetLastMovementInputVector(); // Get character movement direction
    FVector CharacterDirection = GetActorForwardVector(); // Get character direction

    /* Blend strafe animation when moving. */
    if (!MovementDirection.IsNearlyZero())
    {
        /* Calculate the Strafing Rotation which is the Arc Tan difference between the Character's Last Movement Direction and Current Movement Direction */
        float StrafingRotation = FMath::Atan2(MovementDirection.Y, MovementDirection.X) - FMath::Atan2(CharacterDirection.Y, CharacterDirection.X);

        if (FMath::Abs(StrafingRotation) > PI)
        {
            /* Calculate strafe rotation */
            StrafingRotation = StrafingRotation > 0 ? StrafingRotation - PI * 2.0f : StrafingRotation + PI * 2.0f;
        }

        /*Convert StrafingRotation to Degrees*/
        StrafingRotation = FMath::RadiansToDegrees(StrafingRotation);

        /* Apply strafing rotation */
        AnimationInstance->StrafingRotation = StrafingRotation;
    }

    /* Check for walking state */
    bIsWalking ? GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed : GetCharacterMovement()->MaxWalkSpeed = MaxRunSpeed;

    /* Expose animation states to animation instance */
    AnimationInstance->bIsFiring = bIsFiring;
    AnimationInstance->bIsMeleeAttacking = bIsMeleeAttacking;
    AnimationInstance->bIsBlinking = bIsBlinking;

    //Toggle energy charge rate based on player movement. If not moving, charge faster.
    if (bIsMoving == false)
    {
        EnergyComponent->FastCharge = true;
    }
    else
    {
        EnergyComponent->FastCharge = false;
    }
}

bool ABlastRadiusCharacter::GetPickableActor_LineTraceTestByObjectType(EObjectTypeQuery ObjectType)
{
    bool hit = false;
    FVector StartTrace;
    FVector Direction;
    FVector EndTrace;

    SetupRay(StartTrace, Direction, EndTrace);
    FCollisionQueryParams TraceParams;
    TraceParams.AddIgnoredActor(this);
    TraceParams.bTraceComplex = true;
    TraceParams.bReturnPhysicalMaterial = true;

    TEnumAsByte<EObjectTypeQuery> ObjectToTrace = ObjectType;
    TArray<TEnumAsByte<EObjectTypeQuery> > ObjectsToTraceAsByte;
    ObjectsToTraceAsByte.Add(ObjectToTrace);

    FHitResult Hit(ForceInit);
    UWorld* World = GetWorld();
    hit = World->LineTraceTestByObjectType(StartTrace, EndTrace, FCollisionObjectQueryParams(ObjectsToTraceAsByte), TraceParams); // simple trace function
    DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Magenta, true, 1, 0, 5.0f);
    return hit;
}

void ABlastRadiusCharacter::SetupRay(FVector &StartTrace, FVector &Direction, FVector &EndTrace)
{
    StartTrace = GetActorLocation() + GetActorRotation().Vector();
    Direction = GetActorRotation().Vector();
    EndTrace = StartTrace + Direction * 150.0f;
}

//////////////////////////////////////////////////////////////////////////
// States / Conditions
void ABlastRadiusCharacter::OnDeath()
{
    NetMultiCastOnDeath();
}

void ABlastRadiusCharacter::Respawn()
{
    /* Re-enable characters capsule collision */
    if (Role == ROLE_Authority)
    {
        ABlastRadiusGameMode* GameMode = Cast<ABlastRadiusGameMode>(GetWorld()->GetAuthGameMode());
        if (GameMode)
        {
            EnergyComponent->CurrentEnergy = EnergyComponent->MaxEnergy;
            Cast<ABlastRadiusPlayerState>(PlayerState)->SetDamage(0);

            /* Re-enable the actor's tick */
            PrimaryActorTick.bCanEverTick = true;

            /* Respawn Player */
            GameMode->RespawnPlayer(Cast<APlayerController>(GetController()), playerTeam, NetIndex);

            /* Set the player's state to alive */
            bIsDead = false;
        }
    }
}

void ABlastRadiusCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    if (OtherActor)
    {
        // Pickup Battery.
        if (OtherActor->ActorHasTag("Battery"))
        {
            ABlastRadiusBattery* Battery = Cast<ABlastRadiusBattery>(OtherActor);
            if (Battery)
            {
                // Charge Energy
                GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black, "Picked Up Battery");
                EnergyComponent->CurrentEnergy += Battery->Charge;
                Battery->Disable();
            }
        }
    }
}

void ABlastRadiusCharacter::AssignTeams()
{
    if (!GetGameState())
        return;

    TeamOneCount = GetGameState()->TeamOneSize;
    TeamTwoCount = GetGameState()->TeamTwoSize;

    /* Balance team count */
    if (TeamOneCount > TeamTwoCount)
    {
        GetGameState()->TeamTwoSize++;
        playerTeam = 1;
    }
    else if (TeamOneCount < TeamTwoCount)
    {
        GetGameState()->TeamOneSize++;
        playerTeam = 0;
    }
    else
    {
        GetGameState()->TeamTwoSize++;
        playerTeam = 1;
    }
    if (GetPlayerState())
        GetPlayerState()->PlayerTeam = playerTeam;
}

void ABlastRadiusCharacter::AssignNetIndex()
{
    NetIndex = GetGameState()->AuthorityGameMode->GetNumPlayers() - 1;
}

void ABlastRadiusCharacter::UpdateAndCheckPlayer()
{
    if (GetGameState())
    {
        TeamOneCount = GetGameState()->TeamOneSize;
        TeamTwoCount = GetGameState()->TeamTwoSize;
    }
}

void ABlastRadiusCharacter::PostBeginPlay()
{
    if (Role == ROLE_Authority)
        //CALL Multicast_AssignTeamsColor()
        Multicast_AssignTeamsColor();
}

void ABlastRadiusCharacter::Multicast_AssignTeamsColor_Implementation()
{
    if (GetGameState())
    {
        if (playerTeam == 0)
        {
            /* Assign team 1 materials */
            DefaultTPMaterials = GetGameState()->TeamOnePMaterials;
            SkeletalMesh->SetMaterial(0, DefaultTPMaterials);

        }
        else if (playerTeam == 1)
        {
            /* Assign team 2 materials */
            DefaultTPMaterials = GetGameState()->TeamTwoPMaterials;
            SkeletalMesh->SetMaterial(0, DefaultTPMaterials);

        }
    }
}

//////////////////////////////////////////////////////////////////////////
// Input / Actions
void ABlastRadiusCharacter::Move(FVector Direction, float Scale)
{
    /* Check if character is alive. */
    if (bIsDead == false)
    {
        if ((Controller != nullptr) && (Scale != 0.0f))
        {
            // Discern forward direction.
            const FRotator Rotation = GetCameraBoom()->GetTargetRotation(); //Controller->GetControlRotation();
            const FRotator YawRotation(0, Rotation.Yaw, 0);

            if (Direction.X != 0.0f)
            {
                const FVector DirectionX = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

                // Add Movement Input
                AddMovementInput(DirectionX, Scale);
            }
            else if (Direction.Y != 0.0f)
            {
                const FVector DirectionY = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

                // Add Movement Input
                AddMovementInput(DirectionY, Scale);
            }
        }
    }
}

void ABlastRadiusCharacter::LookAt(FVector Direction)
{
    /* Check if character is alive. */
    if (bIsDead == false)
    {
        /* Set Character's direction for server & client. */
        if ((Controller != nullptr) && (Direction != FVector::ZeroVector))
        {
            SetActorRotation(Direction.Rotation());

            Orientation = Direction;
            if (Role < ROLE_Authority)
            {
                ServerLookAt(Orientation);
            }
        }
    }
}

void ABlastRadiusCharacter::ServerBlink_Implementation()
{
    BlinkComponent->Blink();
}

bool ABlastRadiusCharacter::ServerBlink_Validate()
{
    return true;
}

void ABlastRadiusCharacter::Blink()
{
    /* Check if character is alive. */
    if (bIsDead == false)
    {
        /* Check if character has enough energy. */
        if (EnergyComponent->OnCooldown == false)
        {
            bIsBlinking = true;

            /* Blink the character & multi-cast for clients. */
            BlinkComponent->Blink();

            if (Role < ROLE_Authority)
            {
                ServerBlink();
            }

            EnergyComponent->SpendEnergy(BlinkCost);
        }
    }
}

void ABlastRadiusCharacter::LobGrenade()
{
    /* Check if character is alive. */
    if (bIsDead == false)
    {
        /* Check if character has enough energy. */
        if (EnergyComponent->OnCooldown == false)
        {
            bIsFiring = true;

            /* Shoot. */
            if (Role == ROLE_Authority)
                ServerLobGrenade();
            else if (Role < ROLE_Authority)
                ServerLobGrenade();

            /* Spend Energy */
            this->GetEnergyComponent()->SpendEnergy(GrenadeCost);

            /* Play the sound for throwing */
            AudioComponent->SetSound(ThrowSound);
            AudioComponent->Play();
        }
    }
}

void ABlastRadiusCharacter::ServerLobGrenade_Implementation()
{
    /* Check if character is alive. */
    if (bIsDead == false)
    {
        /* Check if character has enough energy. */
        if (EnergyComponent->OnCooldown == false)
        {
            // Raycast to see if something is blocking the grenade.
            if (!GetPickableActor_LineTraceTestByObjectType(EObjectTypeQuery::ObjectTypeQuery1))
            {
                // Attempt to lob a grenade.
                if (GrenadeClass)
                {
                    UWorld* World = GetWorld();
                    if (World)
                    {
                        // Set grenade spawn params.
                        FActorSpawnParameters SpawnParams;
                        SpawnParams.Owner = this;
                        SpawnParams.Instigator = Instigator;

                        // Spawn the projectile at the in front of the character.
                        const FVector SpawnLocation = GetActorLocation() + GetActorRotation().Vector() * 150.0f;
                        const FRotator SpawnRotation = GetActorRotation();
                        ABlastRadiusGrenade* Grenade = World->SpawnActor<ABlastRadiusGrenade>(GrenadeClass, SpawnLocation, SpawnRotation, SpawnParams);

                        if (Grenade)
                        {
                            // Set the projectile's initial trajectory.
                            FVector LaunchDirection = GetActorRotation().Vector();
                            Grenade->LobInDirection(LaunchDirection);
                        }
                    }
                }
            }
        }
    }
}

bool ABlastRadiusCharacter::ServerLobGrenade_Validate()
{
    return true;
}

void ABlastRadiusCharacter::ServerFire_Implementation()
{
    Weapon->Fire();
}

bool ABlastRadiusCharacter::ServerFire_Validate()
{
    return true;
}

void ABlastRadiusCharacter::Fire()
{
    /* Check if character is alive. */
    if (bIsDead == false)
    {
        /* Check if character has enough energy. */
        if (EnergyComponent->OnCooldown == false)
        {
            bIsFiring = true;

            /* Shoot. */
            if (Role == ROLE_Authority)
                Weapon->Fire();
            else if (Role == ROLE_AutonomousProxy)
                ServerFire();

            /* Spend Energy */
            this->GetEnergyComponent()->SpendEnergy(ShootCost);

            /* Play Sound */
            AudioComponent->SetSound(FireSound);
            AudioComponent->Play();
        }
    }
}

void ABlastRadiusCharacter::Melee()
{
    /* Check if character is alive. */
    if (bIsDead == false)
    {
        /* Check if character has enough energy. */
        if (EnergyComponent->OnCooldown == false)
        {
            if (Sword != nullptr)
            {
                bIsMeleeAttacking = true;
                GetWorld()->GetTimerManager().SetTimer(TimerHandle_MeleeTimer, this, &ABlastRadiusCharacter::PutAwaySword, 0.5f, true);
                Sword->Activate();
                EnergyComponent->SpendEnergy(MeleeCost);
            }
        }
    }
}

void ABlastRadiusCharacter::PutAwaySword()
{
    if (SwordClass != nullptr)
        Sword->PutAway();
}

void ABlastRadiusCharacter::NetMultiCastOnDeath_Implementation()
{
    /* Set player dead state */
    bIsDead = true;

    /* Stop ticking while dead */
    PrimaryActorTick.bCanEverTick = false;

    /* Disable character's capsule collision */
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    /* Enable the character's ragdoll */
    SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    SkeletalMesh->SetSimulatePhysics(true);

    /* Play death sound */
    if (DeathSound)
    {
        AudioComponent->SetSound(DeathSound);
        AudioComponent->Play();
    }

    /* Start the delay until respawn */
    GetWorld()->GetTimerManager().SetTimer(TimerHandle_SpawnTimer, this, &ABlastRadiusCharacter::Respawn, SpawnDelay, false);
}

bool ABlastRadiusCharacter::NetMultiCastOnDeath_Validate()
{
    return true;
}

void ABlastRadiusCharacter::ServerLookAt_Implementation(FVector Direction)
{
    SetActorRotation(Direction.Rotation());
}

bool ABlastRadiusCharacter::ServerLookAt_Validate(FVector Direction)
{
    return true;
}

void ABlastRadiusCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ABlastRadiusCharacter, Weapon);
    DOREPLIFETIME(ABlastRadiusCharacter, Grenade);
    DOREPLIFETIME(ABlastRadiusCharacter, Sword);
    DOREPLIFETIME(ABlastRadiusCharacter, Orientation);
    DOREPLIFETIME(ABlastRadiusCharacter, DefaultTPMaterials);
    DOREPLIFETIME(ABlastRadiusCharacter, playerTeam);
}