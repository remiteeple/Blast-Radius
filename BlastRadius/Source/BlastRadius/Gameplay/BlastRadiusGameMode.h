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

    /* Handle new player override from AGameMode */
    virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

    /* Respawn player */
    void RespawnPlayer(APlayerController* NewPlayer, int playerTeam, int NetIndex);

private:
    void HandleNewPlayer(APlayerController* NewPlayer);
#pragma endregion Methods

#pragma region Members
public:
    /* Match Timer */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
        FTimerHandle EndCountDown;

#pragma endregion Members

};



