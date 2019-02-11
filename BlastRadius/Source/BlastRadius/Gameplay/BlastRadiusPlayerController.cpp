// Fill out your copyright notice in the Description page of Project Settings.

#include "BlastRadiusPlayerController.h"
#include "Character/BlastRadiusCharacter.h"
#include "Component/BlinkComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LocalPlayer.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "DrawDebugHelpers.h"

ABlastRadiusPlayerController::ABlastRadiusPlayerController()
{
    //TODO remove before final
    bShowMouseCursor = true;
}

void ABlastRadiusPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Set up gameplay key bindings
    InputComponent->BindAxis("MoveVertical", this, &ABlastRadiusPlayerController::MoveVertical);
    InputComponent->BindAxis("MoveHorizontal", this, &ABlastRadiusPlayerController::MoveHorizontal);

    InputComponent->BindAction("Walk", IE_Pressed, this, &ABlastRadiusPlayerController::WalkPressed);
    InputComponent->BindAction("Walk", IE_Released, this, &ABlastRadiusPlayerController::WalkReleased);

    InputComponent->BindAction("Fire", IE_Pressed, this, &ABlastRadiusPlayerController::FirePressed);
    InputComponent->BindAction("Fire", IE_Released, this, &ABlastRadiusPlayerController::FireReleased);

    InputComponent->BindAction("Aim", IE_Pressed, this, &ABlastRadiusPlayerController::AimPressed);
    InputComponent->BindAction("Aim", IE_Released, this, &ABlastRadiusPlayerController::AimReleased);

    InputComponent->BindAction("Blink", IE_Pressed, this, &ABlastRadiusPlayerController::BlinkPressed);
    InputComponent->BindAction("Blink", IE_Released, this, &ABlastRadiusPlayerController::BlinkReleased);

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

    LookAtMouseCursor();
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
}

void ABlastRadiusPlayerController::FireReleased()
{
    if (Character == nullptr)
        return;
}

void ABlastRadiusPlayerController::AimPressed()
{
    if (Character == nullptr)
        return;
}

void ABlastRadiusPlayerController::AimReleased()
{
    if (Character == nullptr)
        return;
}

void ABlastRadiusPlayerController::BlinkPressed()
{
    if (Character == nullptr)
        return;

    //TODO: determine if we want distance/charging blink, if not... set bIsBlinking to true in BlinkComponent
    //Character->bIsBlinking = true;
    Character->BlinkComponent->Blink(Cast<AActor>(Character));
}

void ABlastRadiusPlayerController::BlinkReleased()
{
    if (Character == nullptr)
        return;
}

void ABlastRadiusPlayerController::LookAtMouseCursor()
{
    if (Character == nullptr)
        return;

    // Trace to see what is under the mouse cursor
    FHitResult TraceOutResult;
    GetHitResultUnderCursor(ECC_Visibility, false, TraceOutResult);
    FVector IntersectVector;

    // Get mouse position on screen
    FVector2D MouseScreenPosition;
    GetMousePosition(MouseScreenPosition.X, MouseScreenPosition.Y);

    // Translate mouse position on 2D screen to 3D world position
    FVector WorldPosition;
    FVector WorldDirection;
    if (UGameplayStatics::DeprojectScreenToWorld(this, MouseScreenPosition, WorldPosition, WorldDirection) == true)
    {
        // Intersect plane with trace hit actor (the ground)
        IntersectVector = FMath::LinePlaneIntersection(WorldPosition, WorldPosition + WorldDirection * HitResultTraceDistance, GetPawn()->GetActorLocation(), FVector::UpVector);
    }

    // Trace down through the aiming plane to see if we hit an actor that we can aim at
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

        // TODO: remove before alpha
        if (bHit)
        {
            DrawDebugLine(GetWorld(), PawnLocation, TargetLocation, FColor(0, 0, 255), false, -1, 0, 10.0f);
            DrawDebugLine(GetWorld(), IntersectVector, TraceOutResult.ImpactPoint, FColor(255, 255, 0), false, -1, 0, 10.0f);
        }
    }

    // Remove vertical axis orientation.
    Direction.Z = 0.0f;

    // Make character look at 
    Character->LookAt(Direction);
}
