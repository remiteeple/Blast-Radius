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

//////////////////////////////////////////////////////////////////////////
// ABlastRadiusCharacter

ABlastRadiusCharacter::ABlastRadiusCharacter() :
    SkeletalMesh(nullptr)
   //AnimationInstance(nullptr)
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
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

void ABlastRadiusCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    /* Retrieve the skeletal mesh */
    SkeletalMesh = GetMesh();

    //Check for skeletal mesh
    if (SkeletalMesh)
    {
        check(SkeletalMesh != nullptr && "Character doesn't have a skeletal mesh!");

        ///* Retrieve the animation instance */
        //AnimationInstance = Cast<UCharacterAnimInstance>(SkeletalMesh->GetAnimInstance());

        //check(AnimationInstance != nullptr && "Character doesn't have animation!")
    }
}

void ABlastRadiusCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void ABlastRadiusCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    /* Handle movement orientation and speed. */
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->MaxWalkSpeed = MaxRunSpeed;
}

//////////////////////////////////////////////////////////////////////////
// Input
void ABlastRadiusCharacter::Move(FVector Direction, float Scale)
{
    AddMovementInput(Direction, Scale);
}