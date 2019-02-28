// Fill out your copyright notice in the Description page of Project Settings.

#include "EnergyComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"


// Sets default values for this component's properties
UEnergyComponent::UEnergyComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

    OnSpend.AddDynamic(this, &UEnergyComponent::SpendEnergy);
}


// Called when the game starts
void UEnergyComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UEnergyComponent::SpendEnergy(float EnergyCost)
{
    CurrentEnergy -= EnergyCost;
    if (CurrentEnergy <= EnergyCost)
    {
        OnCooldown = true;
    }
    
}

void UEnergyComponent::ToggleCoolDown()
{
    OnCooldown = !OnCooldown;
}


// Called every frame
void UEnergyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (CurrentEnergy < MaxEnergy && FastCharge == false)
    {
        CurrentEnergy += RechargeRateSlow;
    }
    else if (CurrentEnergy < MaxEnergy && FastCharge == true)
    {
        CurrentEnergy += RechargeRateFast;
    }
    
    if (CurrentEnergy > 0.f)
    {
        OnCooldown = false;
    }
    
    if(CurrentEnergy > MaxEnergy)
    {
        CurrentEnergy = MaxEnergy;
    }

    CurrentEnergyDisplayValue = FString::SanitizeFloat(CurrentEnergy);

    if (GetOwner())
    {
        //Displays current character energy. For Debugging, remove on HUD implementation.
        GEngine->AddOnScreenDebugMessage(5, 5.f, FColor::Cyan, "Energy %: " + CurrentEnergyDisplayValue);

        //Displays current character energy. For Debugging, remove on HUD implementation.
        GEngine->AddOnScreenDebugMessage(40, 5.f, FColor::Green, FString::Printf(TEXT("Fast Charge: %x"), FastCharge));
    }
	// ...
}

