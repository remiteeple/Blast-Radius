// Fill out your copyright notice in the Description page of Project Settings.

#include "AIBlastRadiusCharacter.h"
#include "NavigationSystem.h"
#include "AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"



AAIBlastRadiusCharacter::AAIBlastRadiusCharacter()
{
    SetActorTickEnabled(true);

    PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sensing Component"));
    PawnSensingComponent->OnSeePawn.AddDynamic(this, &AAIBlastRadiusCharacter::OnPawnSeen);
    //PawnSensingComponent->OnHearNoise.AddDynamic(this, &AAIBlastRadiusCharacter::OnNoiseHeard());

    SetState(EAIState::Idle);
}

void AAIBlastRadiusCharacter::BeginPlay()
{
    Super::BeginPlay();

    DefaultRotation = GetActorRotation(); // Get the default rotation for the actor based on world position.

    // Initiate patrol routine.
    if (bPatrol)
    {
        MoveToNextPatrolPoint();
    }
}

void AAIBlastRadiusCharacter::OnPawnSeen(APawn* SeenPawn)
{
    if (SeenPawn == nullptr)
        return;

    TargetActor = SeenPawn;

    SetState(EAIState::Alerted);
    //DrawDebugSphere() --> Input params GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12, FColor::Red, false, 10.0f

    //Optional
    //UE_LOG(LogTemp, Warning, TEXT("Seen"));

    AController* Controller = GetController();

    if (Controller != nullptr)
    {
        Controller->StopMovement();
        GetWorldTimerManager().ClearTimer(TimerHandle_ResetState);
        GetWorldTimerManager().SetTimer(TimerHandle_ResetState, this, &AAIBlastRadiusCharacter::ResetState, 2.5f);
    }
}

void AAIBlastRadiusCharacter::ResetState()
{
    // Don't reset if state is anything but idle
    if (AIState == EAIState::Alerted || AIState == EAIState::Attacking)
        return;

    // Reset actor rotation
    SetActorRotation(DefaultRotation);

    // Reset patrol
    ResetPatrol();
}

void AAIBlastRadiusCharacter::ResetPatrol()
{
    // Reset target
    TargetActor = nullptr;

    // Reset state
    SetState(EAIState::Idle);

    // Continue patrol
    if (bPatrol)
        MoveToNextPatrolPoint();
}

void AAIBlastRadiusCharacter::SetState(EAIState NewState)
{
    if (AIState == NewState)
        return;

    AIState = NewState;
}

void AAIBlastRadiusCharacter::MoveToNextPatrolPoint()
{
    if (CurrentPatrolPoint == nullptr || CurrentPatrolPoint == SecondPatrolPoint)
    {
        CurrentPatrolPoint = FirstPatrolPoint;
    }
    else
    {
        CurrentPatrolPoint = SecondPatrolPoint;
    }
    
    AController* Controller = GetController();
    UAIBlueprintHelperLibrary::SimpleMoveToActor(Controller, CurrentPatrolPoint);
}

void AAIBlastRadiusCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Move to next patrol point
    if (CurrentPatrolPoint != nullptr)
    {
        FVector Delta = GetActorLocation() - CurrentPatrolPoint->GetActorLocation();
        float Distance = Delta.Size();

        if (Distance < 150.0f)
            MoveToNextPatrolPoint();
    }

    // Rotate towards target
    if (TargetActor != nullptr)
    {
        FVector Location = GetActorLocation();
        FVector TargetLocation = TargetActor->GetActorLocation();
        FVector Direction = TargetLocation - Location;
        Direction.Normalize();
        
        SetActorRotation(FMath::Lerp(GetActorRotation(), Direction.Rotation(), 0.25f));

        // Begin shooting if looking at target
        if (GetActorRotation() == Direction.Rotation() && AIState == EAIState::Alerted)
            SetState(EAIState::Attacking);
    }

    // Fire at target
    if (TargetActor != nullptr && AIState == EAIState::Attacking)
    {
        Fire();
    }
}
