// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "BlastRadiusCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Character/CharacterAnimInstance.h"
#include "Component/BlinkComponent.h"
#include "Component/HealthComponent.h"
#include "Component/EnergyComponent.h"
//#include "Component/MeleeComponent.h"
#include "Weapon/BlastRadiusSword.h"
#include "Weapon/BlastRadiusProjectile.h"
#include "Pickup/BlastRadiusPickup.h"
#include "Pickup/BlastRadiusBattery.h"
#include "Gameplay/BlastRadiusGameStateBase.h"
#include "Gameplay/BlastRadiusPlayerState.h"
#include "Weapon/BlastRadiusWeapon.h"
#include "Gameplay/BlastRadiusPlayerController.h"
#include "Runtime/Engine/Classes/Engine/CollisionProfile.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

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

    // Don't rotate when the controller rotates. Let that just affect the camera.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    GetCharacterMovement()->MaxWalkSpeed = MaxRunSpeed;
    GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
    GetCharacterMovement()->SetJumpAllowed(false);

    // Create a camera boom (pulls in towards the player if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->bEnableCameraLag = true;
    CameraBoom->CameraLagSpeed = 5.0f;
    CameraBoom->CameraLagMaxDistance = 250.0f;
    CameraBoom->bDoCollisionTest = false;

    // Create a follow camera
    TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    TopDownCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
    TopDownCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

    // Setup the health and energy components
    HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
    EnergyComponent = CreateDefaultSubobject<UEnergyComponent>(TEXT("Energy"));
    BlinkComponent = CreateDefaultSubobject<UBlinkComponent>(TEXT("Blink"));

    Sword = nullptr;
    Weapon = nullptr;

    SpawnDelay = 1.0f;

    ProjectileFX = CreateDefaultSubobject<UParticleSystem>(TEXT("Blink Particles"));

    PSC = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MyPSC"));
    PSC->SetupAttachment(RootComponent);

    Tags.Add("Player");

    SetReplicates(true);
    SetReplicateMovement(true);
    
}

void ABlastRadiusCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    /* Retrieve the skeletal mesh */
    //Check for skeletal mesh
    SkeletalMesh = GetMesh();
    if (SkeletalMesh != nullptr)
    {
        check(SkeletalMesh != nullptr && "Character doesn't have a skeletal mesh!");
        /* Retrieve the animation instance */
        AnimationInstance = Cast<UCharacterAnimInstance>(SkeletalMesh->GetAnimInstance());

        //check(AnimationInstance != nullptr && "Character doesn't have animation!")
    }

    /* Retrieve the health component */
    HealthComponent = FindComponentByClass<UHealthComponent>();

    //Check for health component
    if (HealthComponent != nullptr)
    {
        // Subscribe to death event
        HealthComponent->OnDeath.AddDynamic(this, &ABlastRadiusCharacter::NetMultiCastOnDeath);
    }

    /* Retrieve the energy component */
    EnergyComponent = FindComponentByClass<UEnergyComponent>();

    /* Retrieve the blink component */
    BlinkComponent = FindComponentByClass<UBlinkComponent>();

    // Setup overlap
    GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABlastRadiusCharacter::OnOverlapBegin);
}

class ABlastRadiusPlayerState* ABlastRadiusCharacter::GetPlayerState()
{
    return Cast<class ABlastRadiusPlayerState>(this->PlayerState);
}

//TODO Week 7: Return the ABaseGameState
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

    GetWorldTimerManager().SetTimer(UpdateHandle, this, &ABlastRadiusCharacter::UpdateAndCheckPlayer, 0.03333f, true, 0.0f);
    //PostBeginPlay arguments (non looping: PostBeginPlayDelay, this, &ACharacterBase::PostBeginPlay, 1.0f, false
    GetWorldTimerManager().SetTimer(PostBeginPlayDelay, this, &ABlastRadiusCharacter::PostBeginPlay, 1.0f, false);

    if (Role > ROLE_AutonomousProxy)
    {
        //WHOOPS! WON'T WORK IF PLAYERS ARE ALL ADDED AT SAME TIME
        //Set an index reference for this character based on what number player it is in the game
        //SET/ASSIGN NetIndex to GetWorld()->GetGameState()->AuthorityGameMode->GetNumPlayers() - 1
        NetIndex = GetWorld()->GetGameState()->AuthorityGameMode->GetNumPlayers() - 1;
    }
   
}

void ABlastRadiusCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    /* Handle movement and orientation */
    GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	

    float CurrentSpeed = GetVelocity().Size(); // Get character's current speed
    bool bIsMoving = CurrentSpeed > 0.0f && GetCharacterMovement()->IsMovingOnGround(); // Check for character movement

    AnimationInstance->bIsMoving = bIsMoving;
    AnimationInstance->MovementSpeed = bIsMoving ? CurrentSpeed : 0.0f;

    /* Set animation strafing rotation parameter.*/
    FVector MovementDirection = GetLastMovementInputVector(); // Get character movement direction
    FVector CharacterDirection = GetActorForwardVector(); // Get character direction

    /* Blend strafe animation when moving */
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

//////////////////////////////////////////////////////////////////////////
// States / Conditions
void ABlastRadiusCharacter::OnDeath()
{
    ///* Stop ticking while dead */
    //PrimaryActorTick.bCanEverTick = false;

    ///* Disable character's capsule collision */
    //GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    ///* Enable the character's ragdoll */
    //SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    //SkeletalMesh->SetSimulatePhysics(true);

    ///* Start the delay until respawn */
    //GetWorld()->GetTimerManager().SetTimer(TimerHandle_SpawnTimer, this, &ABlastRadiusCharacter::Respawn, SpawnDelay, false);

    NetMultiCastOnDeath();
}

void ABlastRadiusCharacter::Respawn()
{
    /* Re-enable characters capsule collision */
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    /* Turn off collision on the characters mesh */
    SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    /* Turn off the ragdoll on the mesh */
    SkeletalMesh->SetSimulatePhysics(false);

    /* check if the teleport was completed successfully */
    if (TeleportTo(SpawnPoint, GetActorRotation()))
    {
        /* Reset the damage factor */
        HealthComponent->ResetKnockback();
        /* Reset the transform on the mesh */
        SkeletalMesh->ResetRelativeTransform();
        /* Lower the mesh to fit in the capsule */
        SkeletalMesh->AddLocalOffset(FVector(0.0f, 0.0f, -90.0f));
        /* Rotate the mesh to the correct orientation */
        SkeletalMesh->AddLocalRotation(FRotator(0.0f, -90.0f, 0.0f));
        /* Re-attach the mesh to the capsule component */
        SkeletalMesh->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);
    }

    /* Refill energy */
    EnergyComponent->CurrentEnergy = EnergyComponent->MaxEnergy;
    Cast<ABlastRadiusPlayerState>(PlayerState)->SetDamage(0);
    /* Re-enable the actor's tick */
    PrimaryActorTick.bCanEverTick = true;
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

        //IF TeamOneCount is GREATER than TeamTwoCount
        if (TeamOneCount > TeamTwoCount)
        {
            //GET the Game State and Increment TeamTwoSize
            GetGameState()->TeamTwoSize++;
            //SET/Assign playerTeam to 1
            playerTeam = 1;
        }
        else if (TeamOneCount < TeamTwoCount)
        {
            //GET the Game State and Increment TeamOneSize
            GetGameState()->TeamOneSize++;
            //SET/Assign playerTeam to 0
            playerTeam = 0;
        }
        else
        {
            //GET the Game State and Increment TeamTwoSize
            GetGameState()->TeamTwoSize++;
            //SET/Assign playerTeam to 1
            playerTeam = 1;
        }
    //Update the PlayerTeam on the PlayerState
    //CALL GetPlayerState() and SET the PlayerTeam to this instance's playerTeam
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
    if (GetGameState()) //
    {
        //If we're on team one
        //IF playerTEam is 0
        if (playerTeam == 0)
        {
            //If the first person material array for team one isn't null,
            //assign those materials to the first person mesh
            //IF GetGameState()->TeamOnePMaterials.Num() is GREATER than 0
           
                //SET/ASSIGN DefaultTPMaterials to the GameStates's TeamOnePMaterials
                DefaultTPMaterials = GetGameState()->TeamOnePMaterials;
                //CALL ApplyMaterialsToMesh() and pass in GetSkeletalMesh(), DefaultTPMaterials
                SkeletalMesh->SetMaterial(0, DefaultTPMaterials);

        }
        //ELSE IF playerTeam is 1  //Otherwise if we're on team two, do the same as above but for team two        
        else if (playerTeam == 1)
        {
            //IF GetGameState()->TeamTwoPMaterials.Num() is GREATER than 0
          
                //SET/ASSIGN DefaultTPMaterials to the GameStates's TeamTwoPMaterials
                DefaultTPMaterials = GetGameState()->TeamTwoPMaterials;
                SkeletalMesh->SetMaterial(0, DefaultTPMaterials);
                //CALL ApplyMaterialsToMesh() and pass in GetSkeletalMesh(), DefaultTPMaterials
               
        
        }
        //ENDIF
    }
    //ENDIF
}

//////////////////////////////////////////////////////////////////////////
// Input / Actions
void ABlastRadiusCharacter::Move(FVector Direction, float Scale)
{
    if ((Controller != NULL) && (Scale != 0.0f))
    {
        // discern forward direction
        const FRotator Rotation = GetCameraBoom()->GetTargetRotation(); //Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get forward vector
        if (Direction.X != 0.0f)
        {
            const FVector DirectionX = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

            // apply movement
            AddMovementInput(DirectionX, Scale);
        }
        else if (Direction.Y != 0.0f)
        {
            const FVector DirectionY = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

            // apply movement
            AddMovementInput(DirectionY, Scale);
        }
    }
}

void ABlastRadiusCharacter::LookAt(FVector Direction)
{
    if ((Controller != NULL) && Direction != FVector::ZeroVector)
    {
        SetActorRotation(Direction.Rotation());

        // We set our current facing to the direction the actor is facing
        //SET/ASSIGN CurrentFAcing to TargetVector
        Orientation = Direction;
        //IF Role is LESS THAN ROLE_Authority
        if (Role < ROLE_Authority)
        {
            //CALL ServerChangeFacing() and pass in CurrentFacing
            ServerLookAt(Orientation);
        }
    }
}

void ABlastRadiusCharacter::ServerBlink_Implementation()
{
    BlinkComponent->Blink(this);
}

bool ABlastRadiusCharacter::ServerBlink_Validate()
{
    return true;
}

void ABlastRadiusCharacter::Blink()
{
    if (EnergyComponent->OnCooldown == false)
    {
        if (ProjectileFX)
        {
            PSC->SecondsBeforeInactive = 0.5;
            UGameplayStatics::SpawnEmitterAtLocation(this, ProjectileFX, GetActorLocation());
            PSC->SetTemplate(ProjectileFX);
        }

        BlinkComponent->Blink(this);
        ServerBlink();
        EnergyComponent->SpendEnergy(BlinkCost);

        if (ProjectileFX)
        {
            UGameplayStatics::SpawnEmitterAtLocation(this, ProjectileFX, GetActorLocation());
        }
    }
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
    bIsFiring = true;
    if (EnergyComponent->OnCooldown == false)
    {
        ServerFire();
        EnergyComponent->SpendEnergy(ShootCost);
    }
}

void ABlastRadiusCharacter::Melee()
{
    if (Sword != nullptr)
    {
        bIsMeleeAttacking = true;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle_MeleeTimer, this, &ABlastRadiusCharacter::PutAwaySword, 0.5f, true);
        Sword->Activate();
        EnergyComponent->SpendEnergy(MeleeCost);
    }
}

void ABlastRadiusCharacter::PutAwaySword()
{
    if (SwordClass != nullptr)
        Sword->PutAway();
}

void ABlastRadiusCharacter::NetMultiCastOnDeath_Implementation()
{
    /* Stop ticking while dead */
    PrimaryActorTick.bCanEverTick = false;

    /* Disable character's capsule collision */
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    /* Enable the character's ragdoll */
    SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    SkeletalMesh->SetSimulatePhysics(true);

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
    DOREPLIFETIME(ABlastRadiusCharacter, Sword);
    DOREPLIFETIME(ABlastRadiusCharacter, Orientation);
    DOREPLIFETIME(ABlastRadiusCharacter, DefaultTPMaterials);
    DOREPLIFETIME(ABlastRadiusCharacter, playerTeam);
}