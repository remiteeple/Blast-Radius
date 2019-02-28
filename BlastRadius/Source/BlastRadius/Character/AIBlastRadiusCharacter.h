// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BlastRadiusCharacter.h"
#include "AIBlastRadiusCharacter.generated.h"

/**
 * 
 */

class UPawnSensingComponent;

UENUM(BlueprintType)
enum class EAIState : uint8
{
    Idle,
    Alerted,
    Attacking
};

UCLASS()
class BLASTRADIUS_API AAIBlastRadiusCharacter : public ABlastRadiusCharacter
{
	GENERATED_BODY()
public:
    AAIBlastRadiusCharacter();

protected:
    virtual void BeginPlay() override;
	
    /** Components **/
    UPROPERTY(VisibleAnywhere, Category = "Pawn Sensor")
        UPawnSensingComponent* PawnSensingComponent;

    /** Sensing **/
    UFUNCTION()
        void OnPawnSeen(APawn* SeenPawn); // Sight

    /** AI / States **/
    FTimerHandle TimerHandle_ResetState;
    FTimerHandle TimerHandle_ResetPatrol;

    UFUNCTION()
        void ResetState();

    UFUNCTION()
        void ResetPatrol();

    //UPROPERTY(ReplicatedUsing = OnRep_GuardState)
        EAIState AIState;
    //UFUNCTION()
    	//void OnRep_GuardState(); //Gets called when GuardState changes, but only on the clients

    void SetState(EAIState NewState);

    UFUNCTION(BlueprintImplementableEvent, Category = "AI")
        void OnStateChanged(EAIState NewState);

    FRotator DefaultRotation;

    /** Patrolling **/
    UPROPERTY(EditInstanceOnly, Category = "AI")
        bool bPatrol;

    UPROPERTY(EditInstanceOnly, Category = "AI", meta = (EditCondition = "bPatrol"))
        AActor* FirstPatrolPoint;

    UPROPERTY(EditInstanceOnly, Category = "AI", meta = (EditCondition = "bPatrol"))
        AActor* SecondPatrolPoint;

    AActor* CurrentPatrolPoint;

    UFUNCTION()
        void MoveToNextPatrolPoint();

    /** Shooting **/
    UPROPERTY(EditInstanceOnly, Category = "AI")
        bool bCanShoot;

    UPROPERTY(EditInstanceOnly, Category = "AI", meta = (EditCondition = "bCanShoot"))
        float ShootingDelay;

    FTimerHandle TimerHandle_Fire;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

private:
    AActor* TargetActor;	
};
