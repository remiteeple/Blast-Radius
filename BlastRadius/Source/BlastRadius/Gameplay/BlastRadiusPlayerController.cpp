// Fill out your copyright notice in the Description page of Project Settings.

#include "BlastRadiusPlayerController.h"
#include "Character/BlastRadiusCharacter.h"
#include "Component/BlinkComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LocalPlayer.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "DrawDebugHelpers.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

ABlastRadiusPlayerController::ABlastRadiusPlayerController()
{
    bShowMouseCursor = true;
}

void ABlastRadiusPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    /* Fix double click mouse input. */
    SetInputMode(FInputModeGameAndUI());

    /* Aim */
    InputComponent->BindAxis("AimForward", this, &ABlastRadiusPlayerController::AimForward);
    InputComponent->BindAxis("AimRight", this, &ABlastRadiusPlayerController::AimRight);

    /* Camera */
    InputComponent->BindAxis("LookRight", this, &ABlastRadiusPlayerController::LookRight);
    //InputComponent->BindAction("SnapRight", IE_Pressed, this, &ABlastRadiusPlayerController::SnapRightPressed);
    //InputComponent->BindAction("SnapRight", IE_Released, this, &ABlastRadiusPlayerController::SnapRightReleased);

    /* Movement */
    InputComponent->BindAxis("MoveVertical", this, &ABlastRadiusPlayerController::MoveVertical);
    InputComponent->BindAxis("MoveHorizontal", this, &ABlastRadiusPlayerController::MoveHorizontal);

    InputComponent->BindAction("Walk", IE_Pressed, this, &ABlastRadiusPlayerController::WalkPressed);
    InputComponent->BindAction("Walk", IE_Released, this, &ABlastRadiusPlayerController::WalkReleased);

    /* Actions */
    InputComponent->BindAction("Grenade", IE_Pressed, this, &ABlastRadiusPlayerController::GrenadePressed);
    InputComponent->BindAction("Grenade", IE_Released, this, &ABlastRadiusPlayerController::GrenadeReleased);

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

    //if (RightTapPressed)
    //    TimeSinceLastRightTap += DeltaTime;
    //else
    //    TimeSinceLastRightTap = 0.0f;
}

void ABlastRadiusPlayerController::AimForward(float Scale)
{
    if (this != nullptr)
    {
        if (Character == nullptr)
            return;

        LookForwardValue = -Scale;

        /* Rotate the character */
        Character->SetActorRotation(LookDirection);
    }
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

//void ABlastRadiusPlayerController::SnapRightPressed()
//{
//    if (Character == nullptr)
//        return;
//
//    if (RightTapPressed == false)
//    {
//        /* Start double tap timer. */
//        RightTapPressed = true;
//    }
//
//    if (RightTapPressed && TimeSinceLastRightTap >= 0.15f)
//    {
//        /* Add 45 degrees to camera orbit around player. */
//        float CurrentControlRotation = Character->GetController()->GetControlRotation().Yaw;
//        Character->AddControllerYawInput(45.0f);
//    }
//}
//
//void ABlastRadiusPlayerController::SnapRightReleased()
//{
//
//}

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

void ABlastRadiusPlayerController::GrenadePressed()
{
    if (Character == nullptr)
        return;

    Character->LobGrenade();
}

void ABlastRadiusPlayerController::GrenadeReleased()
{
    if (Character == nullptr)
        return;
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

        // Make character look at target.
        Character->LookAt(Direction);

        // Move camera between target and player position.
        //FVector DesiredCameraPosition;
        //DesiredCameraPosition.X = ((TargetLocation.X - Character->GetActorLocation().X) / 2) + Character->GetActorLocation().X;
        //DesiredCameraPosition.Y = ((TargetLocation.Y - Character->GetActorLocation().Y) / 2) + Character->GetActorLocation().Y;
        //DesiredCameraPosition.Z = 1800.0f;

        //Character->GetFollowCamera()->SetWorldLocation(DesiredCameraPosition);

        //Obj.position = new Vector3((WorldPosition.X - Character->GetActorLocation().X) / 2.0f + Character->GetActorLocation().X, (WorldPosition.Y - Character->GetActorLocation().Y) / 2.0f + Character->GetActorLocation().Y, Obj.position.z);

    //    Dist = Vector2.Distance(new Vector2(Obj.position.x, Obj.position.y), new Vector2(Parent.position.x, Parent.position.y));

    //    if (Dist > Radius)
    //    {
    //        var norm = MouseOffset.normalized;
    //        //Obj.position.x = norm.x * Radius + Parent.position.x;
    //        //Obj.position.y = norm.y * Radius + Parent.position.y;
    //        Obj.position = new Vector3(norm.x * Radius + Parent.position.x, norm.y * Radius + Parent.position.y, Obj.position.z);
    //}
    }
}
