// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BlastRadiusGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class BLASTRADIUS_API ABlastRadiusGameStateBase : public AGameState
{
	GENERATED_BODY()
	
public:
#pragma region Members
    /* Materials for the teams (Blue/Orange) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Materials, Replicated)
        class UMaterialInterface* TeamOnePMaterials;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Materials, Replicated)
        class UMaterialInterface* TeamTwoPMaterials;
    /* Check for teams being enabled */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay, Replicated)
        bool TeamsEnabled = true;

    /* Var for team win/loss */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (ClampMin = 0), Replicated)
        int WinningTeam = -1;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (ClampMin = 0), Replicated)
        int LosingTeam = -1;

    /* Vars for team sizes */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (ClampMin = 0), Replicated)
        int TeamOneSize = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (ClampMin = 0), Replicated)
        int TeamTwoSize = 0;
    /* Count down to allowing gameplay */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
        FTimerHandle StartCountDown;

#pragma endregion Members
    

#pragma region Methods
    void BeginPlay() override;
    /* Function to Disable players to start round */
    void StartRound();
    /* Function to re-enable players */
    UFUNCTION(BlueprintCallable)
        void EnablePlayers();
    /* Cast Enabling to clients */
    UFUNCTION(NetMulticast, Reliable, WithValidation)
        void NM_EnablePlayers();
    /* Cast starting the round to clients */
    UFUNCTION(NetMulticast, Reliable, WithValidation)
        void NM_StartRound();
#pragma endregion Methods
};
