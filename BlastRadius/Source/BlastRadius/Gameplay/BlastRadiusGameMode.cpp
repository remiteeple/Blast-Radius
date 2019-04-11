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
    for (AActor* Actor : Players)
    {
        OtherPlayer = Cast<ABlastRadiusCharacter>(Actor);
        if (OtherPlayer->GetController() != NewPlayer)
        {
            OtherPlayers.Add(OtherPlayer);
        }
    }
    for (int i = 0; i < PlayerStarts.Num(); i++)
    {
        float Distance = OtherPlayer->GetDistanceTo(Cast<AActor>(PlayerStarts[i]));
        if (Distance >= 700.0f)
        {
            PreferredStarts.Add(PlayerStarts[i]);
        }
    }

    APawn* pawn = nullptr;
    pawn = SpawnDefaultPawnFor(NewPlayer, PreferredStarts[0]);
    if (pawn)
    {
        ABlastRadiusCharacter* Player = Cast<ABlastRadiusCharacter>(pawn);
        if (Player)
        {
            if (Cast<ABlastRadiusPlayerState>(NewPlayer->PlayerState)->GetLives() > 0)
            {
                /* Assign team & netIndex to pawn passed in */
                Cast<ABlastRadiusCharacter>(pawn)->playerTeam = playerTeam;
                Cast<ABlastRadiusCharacter>(pawn)->NetIndex = NetIndex;

                /* Assign newplayer to pawn & restart them */
                NewPlayer->SetPawn(pawn);
                RestartPlayer(NewPlayer);
            }
            else
            {
                if (playerTeam == 1)
                {

                    

                   ABlastRadiusGameStateBase* GState  = Cast<ABlastRadiusGameStateBase>(GetWorld()->GetGameState());
                   GState->WinningTeam = 0;
                }
                else
                {
                    ABlastRadiusGameStateBase* GState = Cast<ABlastRadiusGameStateBase>(GetWorld()->GetGameState());
                    GState->WinningTeam = 1;
                }
                ABlastRadiusGameStateBase* GState = Cast<ABlastRadiusGameStateBase>(GetWorld()->GetGameState());
                GState->LosingTeam = playerTeam;
                for (int i = 0; i < OtherPlayers.Num(); i++)
                {
                    if (OtherPlayers[i]->GetController())
                    {
                            OtherPlayers[i]->GetPlayerState()->CurrentLives = 0;
                            OtherPlayers[i]->SetActorHiddenInGame(true);
                            OtherPlayers[i]->SetActorLocation(FVector(-10000000.0f, -100000000.0f, -100000000.0f));
                        
                    }
                }
                NewPlayer->DisableInput(NewPlayer);
                NewPlayer->SetActorHiddenInGame(true);
                Cast<AActor>(NewPlayer)->SetActorLocation(FVector(-10000000.0f, -100000000.0f, -100000000.0f));
                GetWorld()->GetTimerManager().SetTimer(EndCountDown, this, &ABlastRadiusGameMode::HandleMatchHasEnded, 3.0f, false);
            }
        }
    }
}

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

