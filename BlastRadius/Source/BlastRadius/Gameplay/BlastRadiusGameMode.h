// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlastRadiusGameMode.generated.h"

UCLASS(minimalapi)
class ABlastRadiusGameMode : public AGameMode
{
    GENERATED_BODY()
        
#pragma region Methods
public:
	ABlastRadiusGameMode();

    void BeginPlay() override;

    /* Handle new player override from AGameMode */
    virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

    /* Respawn player */
    void RespawnPlayer(APlayerController* NewPlayer, int playerTeam, int NetIndex);

    ///* Called when the map transitions to WaitingPostMatch */
    //virtual void HandleMatchHasEnded() override;

    /* Start Count Down Function */
    UFUNCTION(BlueprintCallable)
        void EnablePlayers();

private:
    void HandleNewPlayer(APlayerController* NewPlayer);
#pragma endregion Methods

#pragma region Members
public:
    /* Match Timer */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
        FTimerHandle StartCountDown;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
        FTimerHandle EndCountDown;
    
    /* Var for team win/loss */
    int WinningTeam = -1;
    int LosingTeam = -1;
#pragma endregion Members

#pragma region Getters
public:
    /* Get Winning Team */
    UFUNCTION(BlueprintCallable)
        int GetWinningTeam();
    /* Get Losing Team */
    UFUNCTION(BlueprintCallable)
        int GetLosingTeam();
#pragma endregion Getters
};



