// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "BlastRadiusGameMode.h"
#include "BlastRadius/Character/BlastRadiusCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "BlastRadiusPlayerState.h"
#include "BlastRadiusGameStateBase.h"
#include "EngineGlobals.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
ABlastRadiusGameMode::ABlastRadiusGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ABlastRadiusGameMode::End()
{
    ABlastRadiusPlayerState* PlayerState = Cast<ABlastRadiusPlayerState>(PlayerStateClass);
    if (PlayerState != nullptr)
    {
        
        
        if (PlayerState->GetLives() <= 0)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("WIN"));
        }

    }
}
