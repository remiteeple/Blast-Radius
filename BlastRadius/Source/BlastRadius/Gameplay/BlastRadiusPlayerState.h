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
    int CurrentDamage;

    
    void DecrementLives();
    UFUNCTION()
    int GetLives() { return CurrentLives; };
    void RoundEnd();
	
};
