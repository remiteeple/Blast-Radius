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
    if (CurrentEnergy >= EnergyCost)
    {
        CurrentEnergy -= EnergyCost;

        if (CurrentEnergy > 0.f)
        {
            CurrentEnergy = 0.f;
        }
    }
    
}


// Called every frame
void UEnergyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

