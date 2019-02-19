// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BlastRadiusGameMode.generated.h"

UCLASS(minimalapi)
class ABlastRadiusGameMode : public AGameModeBase
{
    GENERATED_BODY()
        
public:
	ABlastRadiusGameMode();
    void End();
};



