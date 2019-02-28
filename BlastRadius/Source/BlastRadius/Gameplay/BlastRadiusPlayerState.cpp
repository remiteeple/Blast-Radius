// Fill out your copyright notice in the Description page of Project Settings.

#include "BlastRadiusPlayerState.h"
#include "EngineGlobals.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

void ABlastRadiusPlayerState::DecrementLives()
{
    if (CurrentLives != 0)
    {
        CurrentLives--;
    }
    if (CurrentLives <= 0)
    {
        RoundEnd();
    }
    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::FromInt(CurrentLives));
}
void ABlastRadiusPlayerState::RoundEnd()
{
    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("WIN"));
}