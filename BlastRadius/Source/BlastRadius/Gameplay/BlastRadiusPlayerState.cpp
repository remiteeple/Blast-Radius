// Fill out your copyright notice in the Description page of Project Settings.

#include "BlastRadiusPlayerState.h"
#include "EngineGlobals.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "Net/UnrealNetwork.h"
#include "BlastRadiusPlayerController.h"

ABlastRadiusPlayerState::ABlastRadiusPlayerState()
{
    // Enable network replication
    SetReplicates(true);
}

void ABlastRadiusPlayerState::DecrementLives()
{
    if (CurrentLives != 0)
    {
        CurrentLives--;
    }
    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::FromInt(CurrentLives));
}

void ABlastRadiusPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ABlastRadiusPlayerState, CurrentLives);
    DOREPLIFETIME(ABlastRadiusPlayerState, CurrentDamage);
    DOREPLIFETIME(ABlastRadiusPlayerState, CurrentEnergy);
    DOREPLIFETIME(ABlastRadiusPlayerState, PlayerTeam);
}