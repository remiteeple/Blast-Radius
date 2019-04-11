// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "BlastRadiusGameMode.h"
#include "BlastRadius/Character/BlastRadiusCharacter.h"
#include "BlastRadius/Gameplay/BlastRadiusPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "BlastRadiusPlayerState.h"
#include "BlastRadiusGameStateBase.h"
#include "EngineGlobals.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"


ABlastRadiusGameMode::ABlastRadiusGameMode()
{
    PrimaryActorTick.bStartWithTickEnabled = true;
    PrimaryActorTick.bCanEverTick = true;

}

void ABlastRadiusGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
    Super::HandleStartingNewPlayer_Implementation(NewPlayer);
    /* Handle the new player. */
    HandleNewPlayer(NewPlayer);
}

void ABlastRadiusGameMode::RespawnPlayer(APlayerController* NewPlayer, int playerTeam, int NetIndex)
{
    /* Define local actor arrays */
    TArray<AActor*> PlayerStarts;
    TArray<AActor*> Players;
    TArray<AActor*> PreferredStarts;
    TArray <ABlastRadiusCharacter*> OtherPlayers;
    ABlastRadiusCharacter* OtherPlayer = nullptr;
    

    /* Get actors of class type in world */
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABlastRadiusCharacter::StaticClass(), Players);
    //Loop through all actors in the array of players
    for (AActor* Actor : Players)
    {
        ABlastRadiusCharacter* PlayerActor = Cast<ABlastRadiusCharacter>(Actor);
        //Check if actor from loop is the actor being respawned
        if (PlayerActor->GetController() != NewPlayer)
        {
            //Set OtherPlayer for use
            OtherPlayer = Cast<ABlastRadiusCharacter>(PlayerActor);
            break;
        }
    }
    //Loop through all actors in the array of players
    for (AActor* Actor : Players)
    {
        //Get the actor and check if it is not equal to player being respawned
        ABlastRadiusCharacter* OtherPlayerToAdd = Cast<ABlastRadiusCharacter>(Actor);

        if (OtherPlayerToAdd->GetController() != NewPlayer)
        {
            //Add player to Other player array
            OtherPlayers.Add(OtherPlayerToAdd);
        }
    }

    //Loop through Player Starts
    for (int i = 0; i < PlayerStarts.Num(); i++)
    {
        //Check if we have other player
        if (OtherPlayer != nullptr)
        {

            //Get distance between the player start and the other player
            float Distance = OtherPlayer->GetDistanceTo(Cast<AActor>(PlayerStarts[i]));
          
            if (Distance >= 700.0f)
            {
                //Add PlayerStart to Array of preffered starts
                PreferredStarts.Add(PlayerStarts[i]);
            }
        }
    }
    //Create new pawn for use
    APawn* pawn = nullptr;
    //Spawn the new pawn at the first preffered start with the NewPlayer controller
    pawn = SpawnDefaultPawnFor(NewPlayer, PreferredStarts[0]);
    if (pawn)
    {
        ABlastRadiusCharacter* Player = Cast<ABlastRadiusCharacter>(pawn);
        if (Player)
        {
            //If the players lives arent 0 then spawn normally
            if (Cast<ABlastRadiusPlayerState>(NewPlayer->PlayerState)->GetLives() > 0)
            {
                /* Assign team & netIndex to pawn passed in */
                Cast<ABlastRadiusCharacter>(pawn)->playerTeam = playerTeam;
                Cast<ABlastRadiusCharacter>(pawn)->NetIndex = NetIndex;

                /* Assign newplayer to pawn & restart them */
                NewPlayer->SetPawn(pawn);
                RestartPlayer(NewPlayer);
            }
            //If the player's lives are 0 or lower then end match
            else
            {
                if (playerTeam == 1)
                {

                    
                    //Set the winning team in the gamestate to the opposite of the respawned player's team
                   ABlastRadiusGameStateBase* GState  = Cast<ABlastRadiusGameStateBase>(GetWorld()->GetGameState());
                   GState->WinningTeam = 0;
                }
                else
                {
                    //Set the winning team in the gamestate to the opposite of the respawned player's team

                    ABlastRadiusGameStateBase* GState = Cast<ABlastRadiusGameStateBase>(GetWorld()->GetGameState());
                    GState->WinningTeam = 1;
                }
                //Set the losing team in the gamestate to the respawned player's team
                ABlastRadiusGameStateBase* GState = Cast<ABlastRadiusGameStateBase>(GetWorld()->GetGameState());
                GState->LosingTeam = playerTeam;
                //Loop through all otherplayers 
                for (int i = 0; i < OtherPlayers.Num(); i++)
                {
                    //Check if the have a controller
                    if (OtherPlayers[i]->GetController())
                    {
                        //Reduce the players lives to 0
                            OtherPlayers[i]->GetPlayerState()->CurrentLives = 0;
                            //Set them to be hidden
                            OtherPlayers[i]->SetActorHiddenInGame(true);
                            //Move the player below the kill plane
                            OtherPlayers[i]->SetActorLocation(FVector(-10000000.0f, -100000000.0f, -100000000.0f));
                        
                    }
                }
                //Disable the input of the killed player
                NewPlayer->DisableInput(NewPlayer);
                //Hide the new player
                NewPlayer->SetActorHiddenInGame(true);
                //Move the player below the kill plane
                Cast<AActor>(NewPlayer)->SetActorLocation(FVector(-10000000.0f, -100000000.0f, -100000000.0f));
                //Start timer for match end
               // GetWorld()->GetTimerManager().SetTimer(EndCountDown, this, &ABlastRadiusGameMode::HandleMatchHasEnded, 3.0f, false);
            }
        }
    }
}

//void ABlastRadiusGameMode::HandleMatchHasEnded()
//{
//    /*  Super::HandleMatchHasEnded();
//      TArray<AActor*> Players;
//      ABlastRadiusCharacter* Player = nullptr;
//      UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABlastRadiusCharacter::StaticClass(), Players);
//
//      for (AActor* Actor : Players)
//      {
//          Player = Cast<ABlastRadiusCharacter>(Actor);
//          if (Player->GetController())
//          Player->GetController()->DisableInput(Cast<APlayerController>(Player->GetController()));
//      }*/
//}


void ABlastRadiusGameMode::HandleNewPlayer(APlayerController* NewPlayer)
{
    /* Cast NewPlayer to Character */
    ABlastRadiusCharacter* Character = Cast<ABlastRadiusCharacter>(NewPlayer->GetPawn());
    if (Character)
    {
        /* Debug log */
        GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, "Character Logged In");

        /* Assign team and netIndex */
        Character->AssignTeams();
        Character->AssignNetIndex();
    }
}

