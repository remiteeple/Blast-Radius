// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "BlastRadiusGameMode.h"
#include "BlastRadius/Character/BlastRadiusCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "BlastRadiusPlayerState.h"
#include "BlastRadiusGameStateBase.h"
#include "EngineGlobals.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"


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

void ABlastRadiusGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
    //CALL Super Function
    Super::HandleStartingNewPlayer_Implementation(NewPlayer);
    //CALL HandleNewPlayer() pass in NewPlayer
    HandleNewPlayer(NewPlayer);
}

void ABlastRadiusGameMode::RespawnPlayer(APlayerController* NewPlayer, int playerTeam, int NetIndex)
{
    TArray<AActor*> PlayerStarts;
    TArray<AActor*> Players;
    TArray<AActor*> PreferredStarts;
    ABlastRadiusCharacter* Player = nullptr;
    //CALL UGameplayStatics::GetAllActorsOfClass() and pass in GetWorld(), APlayerStart::StaticClass(), PlayerStarts to populate the PlayerStarts TArray
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABlastRadiusCharacter::StaticClass(), Players);
    for (AActor* Actor : Players)
    {
        Player = Cast<ABlastRadiusCharacter>(Actor);
        if (Player->GetController() != NewPlayer)
        {
            break;
        }
    }
    for (int i = 0; i < PlayerStarts.Num(); i++)
    {
        float Distance = Player->GetDistanceTo(Cast<AActor>(PlayerStarts[i]));
            if (Distance >= 500.0f)
            {
                PreferredStarts.Add(PlayerStarts[i]);
                
            }
    }
    APawn* pawn = nullptr;
    //DECLARE a APawn* called pawn and assign it to the return value of SpawnDefaultPawnFor(NewPlayer, PlayerStarts[0]), Look up this Function in the documentation
    
        pawn = SpawnDefaultPawnFor(NewPlayer, PreferredStarts[0]);
 
    //IF the pawn is not nullptr
    if (pawn)
    {
        //IF the pawn is a ACharacterBase type
        if (Cast<ABlastRadiusCharacter>(pawn))
        {
            //SET the pawn's playerTeam to the playerTeam passed in
            Cast<ABlastRadiusCharacter>(pawn)->playerTeam = playerTeam;
            //SET the pawn's NetIndex to the NetIndex passed in
            Cast<ABlastRadiusCharacter>(pawn)->NetIndex = NetIndex;

            //CALL SetPawn() on the NewPlayer and pass in pawn
            NewPlayer->SetPawn(pawn);
            //CALL RestartPlayer() and pass in NewPlayer ..Look up the RestartPlayer() function in the documentation
            RestartPlayer(NewPlayer);

        }
        //ENDIF
    }
    //ENDIF
}

//void ABlastRadiusGameMode::HandleMatchIsWaitingToStart()
//{
//
//}
//
//void ABlastRadiusGameMode::HandleMatchHasStarted()
//{
//
//}
//
//void ABlastRadiusGameMode::HandleMatchHasEnded()
//{
//
//}
//
//bool ABlastRadiusGameMode::ReadyToEndMatch_Implementation()
//{
//    return false;
//}

void ABlastRadiusGameMode::HandleNewPlayer(APlayerController* NewPlayer)
{
    //DECLARE a ACharacterBase* called character and assign it to the Cast of NewPlayer->GetPawn()
    ABlastRadiusCharacter* character = Cast<ABlastRadiusCharacter>(NewPlayer->GetPawn());
    //IF the character is not nullptr
    if (character)
    {
        //Draw a debug message saying character has logged in
        GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, "Character Logged In");
        //CALL AssignTeams() on the character
        character->AssignTeams();
        //CALL AssignNetIndex() on the character
        character->AssignNetIndex();
    }
}

