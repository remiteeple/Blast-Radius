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
    ABlastRadiusPlayerState();
	
#pragma region Members
public:
    /* Playerstate Variables */
    UPROPERTY(EditAnywhere, Category = Lives, Replicated)
        int CurrentLives = 3;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage, Replicated)
        int CurrentDamage = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage, Replicated)
        int CurrentEnergy = 100;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Team, Replicated)
        int PlayerTeam;
#pragma endregion Members

#pragma region Methods
public:
    /* Decrement Lives */
    void DecrementLives();

#pragma endregion Methods

#pragma region Getters
public:
    /* Return Lives */
    UFUNCTION(BlueprintCallable)
        FORCEINLINE int GetLives() { return CurrentLives; }

    /* Return Damage */
    UFUNCTION(BlueprintCallable)
        FORCEINLINE float GetDamage() { return CurrentDamage; }

    /* Return Energy */
    UFUNCTION(BlueprintCallable)
        FORCEINLINE float GetEnergy() { return CurrentEnergy; }
#pragma endregion Getters

#pragma region Setters
    /* Apply Damage */
    FORCEINLINE void IncrementDamage(float Damage) { CurrentDamage += Damage; }
    FORCEINLINE void SetDamage(float Damage) { CurrentDamage = Damage; }
	FORCEINLINE void SetEnergy(float Energy) { CurrentEnergy = Energy; }
#pragma endregion Setters
};
