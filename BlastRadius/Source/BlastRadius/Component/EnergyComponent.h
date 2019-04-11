// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnergyComponent.generated.h"

//OnSpendEnergy delegate signature declaration
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpendEnergy, float, EnergyCost);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BLASTRADIUS_API UEnergyComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UEnergyComponent();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma region Members
public:
    //Maximum Energy of the character.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
        float MaxEnergy = 100.f;

    //Minimum Energy of the character
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
        float MinEnergy = -20.f;

    //Current Energy of the character.
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Attributes)
        float CurrentEnergy = 100.f;

    //Display value of current energy.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
        FString CurrentEnergyDisplayValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
        float RechargeRateSlow = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
        float RechargeRateFast = 0.4f;

    //Bool for checking if the timer is finished.
    bool OnCooldown = false;

    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Attributes)
        bool FastCharge = false;
#pragma endregion Members

#pragma region Methods
public:
    //Delegate function call initialization.
    UPROPERTY(BluePrintAssignable)
        FOnSpendEnergy OnSpend;

    UFUNCTION(BluePrintCallable)
        void SpendEnergy(float EnergyCost);

    UFUNCTION(BluePrintCallable)
        void ToggleCoolDown();

#pragma endregion Methods

#pragma region Getters
    UFUNCTION(BluePrintCallable)
        FORCEINLINE float GetCurrentEnergy() { return CurrentEnergy; }
#pragma endregion Getters
};
