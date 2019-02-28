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
#include "Gameplay/BlastRadiusGameStateBase.h"
#include "Gameplay/BlastRadiusPlayerState.h"
#include "Weapon/BlastRadiusWeapon.h"
#include "Gameplay/BlastRadiusPlayerController.h"
#include "Runtime/Engine/Classes/Engine/CollisionProfile.h"

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
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 5.0f;
	CameraBoom->CameraLagMaxDistance = 250.0f;
    
	// Create a follow camera
    TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    TopDownCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
    TopDownCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

    MuzzleOffset = FVector(100, 0, 0);

    /*HealthPercentage = 0.0;
    Energy = 100;*/


    //BlinkComponent = CreateDefaultSubobject<UBlinkComponent>(TEXT("Blink"));
    //MeleeComponent = CreateDefaultSubobject<UMeleeComponent>(TEXT("Melee"));

    //Setup the health and energy components
    HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
    EnergyComponent = CreateDefaultSubobject<UEnergyComponent>(TEXT("Energy"));

    Sword = nullptr;
    Weapon = nullptr;

    SpawnDelay = 1.0f;
    //HealthPercentage = 0.0;
    //Energy = 100;

    Tags.Add("Player");
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
        HealthComponent->OnDeath.AddDynamic(this, &ABlastRadiusCharacter::OnDeath);
    }

    /* Retrieve the energy component */
    EnergyComponent = FindComponentByClass<UEnergyComponent>();

    /* Retrieve the blink component */
    BlinkComponent = FindComponentByClass<UBlinkComponent>();
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
void ABlastRadiusCharacter::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

}

void ABlastRadiusCharacter::OnDeath()
{  /* This check isnt useful currently */
   // check(HealthComponent->CurrentHealth > 0.0f && "Called OnDeath() while alive!");

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
        GetCapsuleComponent()->SetLinearDamping(10000000000000);
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

    /**Refill energy **/
    EnergyComponent->CurrentEnergy = EnergyComponent->MaxEnergy;
    /* Re-enable the actor's tick */
    PrimaryActorTick.bCanEverTick = true;
}

//////////////////////////////////////////////////////////////////////////
// Input / Actions
void ABlastRadiusCharacter::Move(FVector Direction, float Scale)
{
    if (Scale != 0.0f)
    {
        AddMovementInput(Direction, Scale);
    }
}

void ABlastRadiusCharacter::Blink()
{
    if (EnergyComponent->OnCooldown == false)
    {
        BlinkComponent->Blink(this);
        EnergyComponent->SpendEnergy(BlinkCost);
    }
    
}

void ABlastRadiusCharacter::Fire()
{
    bIsFiring = true;
    if (EnergyComponent->OnCooldown == false)
    {
        Weapon->Fire();
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

void ABlastRadiusCharacter::LookAt(FVector Direction)
{
    SetActorRotation(Direction.Rotation());
}

//////////////////////////////////////////////////////////////////////////
// Getters

class ABlastRadiusPlayerState* ABlastRadiusCharacter::GetPlayerState() 
{ 
    return Cast<ABlastRadiusPlayerState>(PlayerState); 
}

class ABlastRadiusGameStateBase* ABlastRadiusCharacter::GetGameState()
{
    return Cast<class ABlastRadiusGameStateBase>(GetWorld()->GetGameState()); 
}
