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
    int CurrentLives;
    int CurrentDamage;

    void DecrementLives();
    int GetLives() { return CurrentLives; };
	
};
