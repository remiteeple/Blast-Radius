// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "BlastRadiusCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Character/CharacterAnimInstance.h"
#include "Component/BlinkComponent.h"
#include "Component/HealthComponent.h"
#include "Component/EnergyComponent.h"
#include "Weapon/BlastRadiusProjectile.h"
#include "Gameplay/BlastRadiusPlayerController.h"

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
}

void ABlastRadiusCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	/* Retrieve the skeletal mesh */
	SkeletalMesh = GetMesh();

    //Check for skeletal mesh
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
}

void ABlastRadiusCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    /* Handle movement and orientation */
    GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	

    float CurrentSpeed = GetVelocity().Size(); // Get character's current speed
    bool bIsMoving = CurrentSpeed > 0.0f && GetCharacterMovement()->IsMovingOnGround(); // Check for character movement

    //AnimationInstance->bIsMoving = bIsMoving;
    //AnimationInstance->MovementSpeed = bIsMoving ? CurrentSpeed : 0.0f;

    ///* Set animation strafing rotation paremeter.*/
    //FVector MovementDirection = GetLastMovementInputVector(); // Get character movement direction
    //FVector CharacterDirection = GetActorForwardVector(); // Get character direction

    ///*We need to set the Strafeing Rotation on the AnimationInstance to blend the movement animation when moving*/
    //if (!MovementDirection.IsNearlyZero())
    //{
    //    /*Calculate the Strafing Rotation which is the Arc Tan difference between the Character's Last Movement Direction and Current Movement Direction*/
    //    //DECLARE a float called StrafingRotation and SET it to FMath::Atan2(MovementDirection.Y, MovementDirection.X) - FMath::Atan2(CharacterDirection.Y, CharacterDirection.X)
    //    float StrafingRotation = FMath::Atan2(MovementDirection.Y, MovementDirection.X) - FMath::Atan2(CharacterDirection.Y, CharacterDirection.X);

    //    //IF the Absolute value of the StrafingRotation is greater than PI FMath::Abs(StrafingRotation) > PI
    //    if (FMath::Abs(StrafingRotation) > PI)
    //    {
    //        //SET StrafingRotation, If StrafingRotation is greater than 0, then set it to (StrafingRotation - PI * 2.0f), otherwise (StrafingRotation + PI * 2.0f) --> Ternary
    //        StrafingRotation = StrafingRotation > 0 ? StrafingRotation - PI * 2.0f : StrafingRotation + PI * 2.0f;
    //    }

    //    /*Convert StrafingRotation to Degrees*/
    //    StrafingRotation = FMath::RadiansToDegrees(StrafingRotation);

    //    //SET the AnimationInstance's StrafingRotation to the local StrafingRotation
    //    AnimationInstance->StrafingRotation = StrafingRotation;
    //}

    // Check for walking state
    bIsWalking ? GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed : GetCharacterMovement()->MaxWalkSpeed = MaxRunSpeed;

    /* Assign animation instances based on local states */
    //AnimationInstance->bIsAiming = bIsAiming;
    //AnimationInstance->bIsBlinking = bIsBlinking;

}

//////////////////////////////////////////////////////////////////////////
// States / Conditions
void ABlastRadiusCharacter::OnDeath()
{
    check(HealthComponent->CurrentHealth > 0.0f && "Called OnDeath() while alive!");

    /* Stop ticking while dead */
    PrimaryActorTick.bCanEverTick = false;

    /* Disable character's capsule collision */
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    /* Enable the character's ragdoll */
    SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    SkeletalMesh->SetSimulatePhysics(true);
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
    BlinkComponent->Blink(this);
}

void ABlastRadiusCharacter::Aim(bool Toggle)
{
    /* Set appropriate state */
    bIsAiming = true;
}

void ABlastRadiusCharacter::Shoot()
{
    // Attempt to fire a projectile.
    if (ProjectileClass)
    {

        // Transform MuzzleOffset from camera space to world space.
        FVector MuzzleLocation = this->GetActorLocation(); 
        FRotator MuzzleRotation = this->GetActorRotation();
        UWorld* World = GetWorld();
        if (World)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            SpawnParams.Instigator = Instigator;
            // Spawn the projectile at the muzzle.
            ABlastRadiusProjectile* Projectile = World->SpawnActor<ABlastRadiusProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
            if (Projectile)
            {
                // Set the projectile's initial trajectory.
                FVector LaunchDirection = MuzzleRotation.Vector();
                Projectile->FireInDirection(LaunchDirection);
            }
        }
    }
}

void ABlastRadiusCharacter::LookAt(FVector Direction)
{
    SetActorRotation(Direction.Rotation());
}