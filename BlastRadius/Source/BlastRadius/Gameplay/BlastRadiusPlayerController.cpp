// Fill out your copyright notice in the Description page of Project Settings.

#include "BlastRadiusPlayerController.h"
#include "Character/BlastRadiusCharacter.h"
#include "Component/BlinkComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LocalPlayer.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "DrawDebugHelpers.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

ABlastRadiusPlayerController::ABlastRadiusPlayerController()
{
    bShowMouseCursor = true;
}

void ABlastRadiusPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Set up gameplay key bindings
    InputComponent->BindAxis("AimForward", this, &ABlastRadiusPlayerController::AimForward);
    InputComponent->BindAxis("AimRight", this, &ABlastRadiusPlayerController::AimRight);

    InputComponent->BindAxis("LookRight", this, &ABlastRadiusPlayerController::AimRight);

    InputComponent->BindAxis("MoveVertical", this, &ABlastRadiusPlayerController::MoveVertical);
    InputComponent->BindAxis("MoveHorizontal", this, &ABlastRadiusPlayerController::MoveHorizontal);

    InputComponent->BindAction("Walk", IE_Pressed, this, &ABlastRadiusPlayerController::WalkPressed);
    InputComponent->BindAction("Walk", IE_Released, this, &ABlastRadiusPlayerController::WalkReleased);

    InputComponent->BindAction("Fire", IE_Pressed, this, &ABlastRadiusPlayerController::FirePressed);
    InputComponent->BindAction("Fire", IE_Released, this, &ABlastRadiusPlayerController::FireReleased);

    InputComponent->BindAction("Blink", IE_Pressed, this, &ABlastRadiusPlayerController::BlinkPressed);
    InputComponent->BindAction("Blink", IE_Released, this, &ABlastRadiusPlayerController::BlinkReleased);

    InputComponent->BindAction("Melee", IE_Pressed, this, &ABlastRadiusPlayerController::MeleePressed);
    InputComponent->BindAction("Melee", IE_Released, this, &ABlastRadiusPlayerController::MeleeReleased);
}

void ABlastRadiusPlayerController::AcknowledgePossession(APawn* PossesedPawn)
{
    Super::AcknowledgePossession(PossesedPawn);

    Character = Cast<ABlastRadiusCharacter>(PossesedPawn);
    check(Character != nullptr && "Player Controller did not receive a valid ABlastRadiusCharacter!");
}

void ABlastRadiusPlayerController::BeginPlay()
{
    Super::BeginPlay();
}

void ABlastRadiusPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Mouse Aim
    LookAtMouseCursor(DeltaTime);

    // Gamepad Aim
    ThumbStickDir = FVector(LookForwardValue, LookRightValue, 0.0f);
    if (ThumbStickDir != FVector::ZeroVector)
    {
        LookDirection = UKismetMathLibrary::MakeRotationFromAxes(ThumbStickDir, FVector(0, 0, 0), FVector(0, 0, 0));
        Character->SetActorRotation(LookDirection);
    }
}

void ABlastRadiusPlayerController::AimForward(float Scale)
{
    if (Character == nullptr)
        return;

    LookForwardValue = -Scale;

    /* Rotate the character */
    Character->SetActorRotation(LookDirection);
}

void ABlastRadiusPlayerController::AimRight(float Scale)
{
    if (Character == nullptr)
        return;

    LookRightValue = Scale;

    /* Rotate the character */
    Character->SetActorRotation(LookDirection);
}

void ABlastRadiusPlayerController::LookRight(float Scale)
{
    if (Character == nullptr)
        return;

    /* Orbit camera around character */
    Character->AddControllerYawInput(Scale * CameraSensitivity * GetWorld()->GetDeltaSeconds());
}

void ABlastRadiusPlayerController::MoveVertical(float Scale)
{
    if (Character == nullptr)
        return;

    /* Move the character along the X-axis */
    Character->Move(FVector(1.0f, 0.0f, 0.0f), Scale);
}

void ABlastRadiusPlayerController::MoveHorizontal(float Scale)
{
    if (Character == nullptr)
        return;

    /* Move the character along the Y-axis */
    Character->Move(FVector(0.0f, 1.0f, 0.0f), Scale);
}

void ABlastRadiusPlayerController::WalkPressed()
{
    if (Character == nullptr)
        return;

    Character->bIsWalking = true;
}

void ABlastRadiusPlayerController::WalkReleased()
{
    if (Character == nullptr)
        return;


    Character->bIsWalking = false;
}

void ABlastRadiusPlayerController::FirePressed()
{
    if (Character == nullptr)
        return;

    Character->Fire();
}

void ABlastRadiusPlayerController::FireReleased()
{
    if (Character == nullptr)
        return;
}

void ABlastRadiusPlayerController::BlinkPressed()
{
    if (Character == nullptr)
        return;

    Character->Blink();
}

void ABlastRadiusPlayerController::BlinkReleased()
{
    if (Character == nullptr)
        return;

    //Character->bIsBlinking = false;
}

void ABlastRadiusPlayerController::MeleePressed()
{
    if (Character == nullptr)
        return;

    Character->Melee();
}

void ABlastRadiusPlayerController::MeleeReleased()
{
    if (Character == nullptr)
        return;

    //Character->bIsMeleeAttacking = false;
}

void ABlastRadiusPlayerController::LookAtMouseCursor(float DeltaTime)
{
    if (Character == nullptr)
        return;

    // Trace to see what is under the mouse cursor
    FHitResult TraceOutResult;
    GetHitResultUnderCursor(ECC_Visibility, false, TraceOutResult);
    FVector IntersectVector;

    // Get mouse position on screen
    FVector2D MousePosition;
    GetMousePosition(MousePosition.X, MousePosition.Y);

    // Translate mouse position on 2D screen to 3D world position
    FVector WorldPosition;
    FVector WorldDirection;
    if (UGameplayStatics::DeprojectScreenToWorld(this, MousePosition, WorldPosition, WorldDirection) == true)
    {
        IntersectVector = FMath::LinePlaneIntersection(WorldPosition, WorldPosition + WorldDirection * HitResultTraceDistance, Character->GetActorLocation(), FVector::UpVector);
    }

    FCollisionQueryParams CollisionQueryParams(NAME_Actor, true);
    bool bHit = GetWorld()->LineTraceSingleByChannel(TraceOutResult, IntersectVector, IntersectVector - FVector::UpVector * HitResultTraceDistance, ECC_Pawn, CollisionQueryParams);

    // If we hit something aim set that as our aim direction
    FVector Direction;
    FVector TargetLocation;

    if (bHit == true) // If trace hits an actor
    {
        // Location = the trace impact position
        TargetLocation = TraceOutResult.ImpactPoint;
    }
    else if (bHit == false) // If trace does not hit an actor
    {
        // Location = the generic intersection vector of the plane
        TargetLocation = IntersectVector;
    }

    // If the Location is not zero (null)
    if (TargetLocation != FVector::ZeroVector)
    {
        FVector PawnLocation = Character->GetActorLocation();
        Direction = TargetLocation - PawnLocation;

        if (bHit)
        {
            DrawDebugLine(GetWorld(), PawnLocation, TargetLocation, FColor(0, 0, 255), false, -1, 0, 10.0f);
            DrawDebugLine(GetWorld(), IntersectVector, TraceOutResult.ImpactPoint, FColor(255, 255, 0), false, -1, 0, 10.0f);
        }

        // Remove vertical axis orientation.
        Direction.Z = 0.0f;

        // Make character look at 
        Character->LookAt(Direction);
    }

    //// Create vectors for Mouse input
    //FVector MouseLocation;
    //FVector MouseDircetion;

    //// Translate mouse position on 2D screen to 3D world position
    //bool Success = DeprojectMousePositionToWorld(MouseLocation, MouseDircetion);

    //// If translation succeeded
    //if (Success)
    //{
    //    // Define trace
    //    float TraceDistance = 10000.0f;
    //    FVector TraceBegin = MouseLocation;
    //    FVector TraceEnd = MouseLocation + MouseDircetion * TraceDistance;

    //    // Define plane
    //    FVector PlaneOrigin = Character->GetActorForwardVector();
    //    FVector PlaneNormal = FVector(0.0f, 0.0f, 1.0f);

    //    // Get planar intersect for floor
    //    FVector LookLocation = FMath::LinePlaneIntersection(TraceBegin, TraceEnd, PlaneOrigin, PlaneNormal);

    //    // Get direction for character aim
    //    FRotator LookRotation = (LookLocation - Character->GetActorLocation()).Rotation();

    //    // Nullify non-used axes
    //    LookRotation.Pitch = 0.0f;
    //    LookRotation.Roll = 0.0f;
    //    LookLocation.Z = Character->GetActorLocation().Z;

    //    // TODO: remove before alpha build
    //    DrawDebugLine(GetWorld(), Character->GetActorLocation(), LookLocation, FColor(0, 0, 255), false, -1, 0, 10.0f);

    //    // Rotate character
    //    Character->SetActorRotation(LookRotation);
    //}
}
