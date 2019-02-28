// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BlastRadiusPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BLASTRADIUS_API ABlastRadiusPlayerState : public APlayerState
{
	GENERATED_BODY()
	
	
public:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Lives)
    int CurrentLives = 3;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
    int CurrentDamage = 0;

    
    void DecrementLives();
    UFUNCTION()
    FORCEINLINE int GetLives() { return CurrentLives; }
    UFUNCTION()
    FORCEINLINE float GetDamage() { return CurrentDamage; }
    FORCEINLINE void IncrementDamage(float Damage) { CurrentDamage += Damage; }
    FORCEINLINE void SetDamage(float Damage) { CurrentDamage = Damage; }
    void RoundEnd();
};
