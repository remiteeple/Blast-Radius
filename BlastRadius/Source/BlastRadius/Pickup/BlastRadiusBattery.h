// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup/BlastRadiusPickup.h"
#include "BlastRadiusBattery.generated.h"

/**
 * 
 */
UCLASS()
class BLASTRADIUS_API ABlastRadiusBattery : public ABlastRadiusPickup
{
	GENERATED_BODY()
	
public:
    ABlastRadiusBattery();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battery")
        float Charge;

    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
};