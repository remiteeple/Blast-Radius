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
    PrimaryActorTick.bStartWithTickEnabled = true;
    PrimaryActorTick.bCanEverTick = true;
 
}

//**** Failed attempts at getting the playerstate to get the player's lives ****//
//void ABlastRadiusGameMode::StartPlay()
//{
//    Super::StartPlay();
//    ABlastRadiusCharacter* Player = Cast<ABlastRadiusCharacter>(Cast<ABlastRadiusGameStateBase>(GameStateClass)->PlayerArray[0]);
//    BlastRadiusState = Player->GetPlayerState();
//    if (PlayerStateClass != nullptr)
//        BlastRadiusState = Cast<ABlastRadiusPlayerState>(PlayerStateClass);
//
//}
//void ABlastRadiusGameMode::Tick(float DeltaTime)
//{
//    Super::Tick(DeltaTime);
//    
//    if (BlastRadiusState->GetLives() <= 0)
//    {
//        RoundEnd();
//    }
//}
//
//void ABlastRadiusGameMode::RoundEnd()
//{
//    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("WIN"));
//}
