// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup/BlastRadiusPickup.h"
#include "BlastRadiusHealthKit.generated.h"

/**
 * 
 */
UCLASS()
class BLASTRADIUS_API ABlastRadiusHealthKit : public ABlastRadiusPickup
{
	GENERATED_BODY()
	
public:
    ABlastRadiusHealthKit();

    /* Value to charge the player's battery by */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
        float HealPercent = 0.25f;
};
