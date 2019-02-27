// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnergyComponent.generated.h"

//OnSpendEnergy delegate signature declaration
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpendEnergy, float, EnergyCost);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTRADIUS_API UEnergyComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnergyComponent();

    //Maximum Energy of the character.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
        float MaxEnergy = 100.f;

    //Current Energy of the character.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
        float CurrentEnergy = 100.f;

    //Display value of current energy.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
        FString CurrentEnergyDisplayValue;

    //Delegate function call initialization.
    FOnSpendEnergy OnSpend;

    //Bool for checking if the timer is finished.
    bool OnCooldown = false;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

    UFUNCTION(BluePrintCallable)
        void SpendEnergy(float EnergyCost);

    UFUNCTION(BluePrintCallable)
        void ToggleCoolDown();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
	
};
