// Fill out your copyright notice in the Description page of Project Settings.

#include "EnergyComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "UnrealNetwork.h"
#include "Gameplay/BlastRadiusPlayerState.h"
#include "Character/BlastRadiusCharacter.h"
// Sets default values for this component's properties
UEnergyComponent::UEnergyComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

    OnSpend.AddDynamic(this, &UEnergyComponent::SpendEnergy);
}

// Called when the game starts
void UEnergyComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UEnergyComponent::SpendEnergy(float EnergyCost)
{
    CurrentEnergy -= EnergyCost;
    if (CurrentEnergy <= EnergyCost)
    {
        OnCooldown = true;
    }
    //Get the player state to modify current player damage
    ABlastRadiusPlayerState* PlayerState = Cast<ABlastRadiusCharacter>(GetOwner())->GetPlayerState();
    if (PlayerState)
         PlayerState->CurrentEnergy = CurrentEnergy;
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
    else if (CurrentEnergy < MinEnergy)
    {
        CurrentEnergy = MinEnergy;
    }

    CurrentEnergyDisplayValue = FString::SanitizeFloat(CurrentEnergy);

    if (GetOwner())
    {
        // Displays current character energy. For Debugging, remove on HUD implementation.
        GEngine->AddOnScreenDebugMessage(5, 5.f, FColor::Cyan, "Energy %: " + CurrentEnergyDisplayValue);

        // Displays current character energy. For Debugging, remove on HUD implementation.
        GEngine->AddOnScreenDebugMessage(40, 5.f, FColor::Green, FString::Printf(TEXT("Fast Charge: %x"), FastCharge));
    }

    // Get the player state to modify current player damage
    ABlastRadiusPlayerState* PlayerState = Cast<ABlastRadiusCharacter>(GetOwner())->GetPlayerState();
    if (PlayerState)
        PlayerState->CurrentEnergy = CurrentEnergy;
}

void UEnergyComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UEnergyComponent, CurrentEnergy);
    DOREPLIFETIME(UEnergyComponent, FastCharge);
}