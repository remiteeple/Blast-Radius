// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BlastRadiusGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class BLASTRADIUS_API ABlastRadiusGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
	
public:
    int GetLives() { return 1; };
   
};
