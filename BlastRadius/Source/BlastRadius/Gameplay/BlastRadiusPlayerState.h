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
    UPROPERTY(EditAnywhere, Category = Lives, Replicated)
    int CurrentLives = 3;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage, Replicated)
    int CurrentDamage = 0;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated)
        int PlayerTeam;
    
    void DecrementLives();
    UFUNCTION(BlueprintCallable)
    FORCEINLINE int GetLives() { return CurrentLives; }
    UFUNCTION(BlueprintCallable)
    FORCEINLINE float GetDamage() { return CurrentDamage; }
    //FORCEINLINE float GetDamage() { return CurrentEnergy; }

    FORCEINLINE void IncrementDamage(float Damage) { CurrentDamage += Damage; }
    FORCEINLINE void SetDamage(float Damage) { CurrentDamage = Damage; }
    void RoundEnd();
};
