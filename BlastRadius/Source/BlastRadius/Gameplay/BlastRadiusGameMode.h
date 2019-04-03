// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlastRadiusGameMode.generated.h"

UCLASS(minimalapi)
class ABlastRadiusGameMode : public AGameMode
{
    GENERATED_BODY()
        
public:
	ABlastRadiusGameMode();

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay)
        int TeamOneScore = 0;

    //TODO Week 7:Team Two's Score
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay)
        int TeamTwoScore = 0;

    //TODO Week 7: Override from GameMode to handle when a new player logs in
    virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

    //TODO Week 7: Respawn the player that has died
    void RespawnPlayer(APlayerController* NewPlayer, int playerTeam, int NetIndex);

    ///** Called when the state transitions to WaitingToStart */
    //virtual void HandleMatchIsWaitingToStart() override;

    ///** Called when the state transitions to InProgress */
    //virtual void HandleMatchHasStarted() override;

    ///** Called when the map transitions to WaitingPostMatch */
    //virtual void HandleMatchHasEnded() override;

    ///** @return true if ready to End Match. */
    //virtual bool ReadyToEndMatch_Implementation() override;

private:
    //TODO Week 7: Handle the new player
    void HandleNewPlayer(APlayerController* NewPlayer);
    
};



