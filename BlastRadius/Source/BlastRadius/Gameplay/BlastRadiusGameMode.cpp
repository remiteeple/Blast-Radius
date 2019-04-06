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

    NewPlayer->DisableInput(NewPlayer);

    //CALL HandleNewPlayer() pass in NewPlayer
    HandleNewPlayer(NewPlayer);
}

int ABlastRadiusGameMode::GetWinningTeam()
{
    return WinningTeam;
}

int ABlastRadiusGameMode::GetLosingTeam()
{
    return LosingTeam;
}

void ABlastRadiusGameMode::RespawnPlayer(APlayerController* NewPlayer, int playerTeam, int NetIndex)
{


    TArray<AActor*> PlayerStarts;
    TArray<AActor*> Players;
    TArray<AActor*> PreferredStarts;
    TArray <ABlastRadiusCharacter*> OtherPlayers;
    ABlastRadiusCharacter* OtherPlayer = nullptr;
    //CALL UGameplayStatics::GetAllActorsOfClass() and pass in GetWorld(), APlayerStart::StaticClass(), PlayerStarts to populate the PlayerStarts TArray
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
    //DECLARE a APawn* called pawn and assign it to the return value of SpawnDefaultPawnFor(NewPlayer, PlayerStarts[0]), Look up this Function in the documentation
    pawn = SpawnDefaultPawnFor(NewPlayer, PreferredStarts[0]);


    //IF the pawn is not nullptr
    if (pawn)
    {
        ABlastRadiusCharacter* Player = Cast<ABlastRadiusCharacter>(pawn);
        //IF the pawn is a ACharacterBase type
        if (Player)
        {
            if (Cast<ABlastRadiusPlayerState>(NewPlayer->PlayerState)->GetLives() > 0)
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
            else
            {
                if (playerTeam == 1)
                {
                    WinningTeam = 0;
                }
                else
                {
                    WinningTeam = 1;
                }
                LosingTeam = playerTeam;
                for (int i = 0; i < OtherPlayers.Num(); i++)
                {
                    if (OtherPlayers[i]->GetController())
                    {
                        
                            OtherPlayers[i]->GetPlayerState()->CurrentLives = 0;
                            OtherPlayers[i]->SetActorHiddenInGame(true);
                            OtherPlayers[i]->SetActorLocation(FVector(-10000000.0f, -100000000.0f, -100000000.0f));
                        
                        //OtherPlayers[i]->SetActorLocation()
                    }
                }
                NewPlayer->DisableInput(NewPlayer);
                NewPlayer->SetActorHiddenInGame(true);
                Cast<AActor>(NewPlayer)->SetActorLocation(FVector(-10000000.0f, -100000000.0f, -100000000.0f));
                GetWorld()->GetTimerManager().SetTimer(EndCountDown, this, &ABlastRadiusGameMode::HandleMatchHasEnded, 3.0f, false);

            }
            //ENDIF
        }
        //ENDIF
    }
}

void ABlastRadiusGameMode::HandleMatchHasEnded()
{
    /*  Super::HandleMatchHasEnded();
      TArray<AActor*> Players;
      ABlastRadiusCharacter* Player = nullptr;
      UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABlastRadiusCharacter::StaticClass(), Players);

      for (AActor* Actor : Players)
      {
          Player = Cast<ABlastRadiusCharacter>(Actor);
          if (Player->GetController())
          Player->GetController()->DisableInput(Cast<APlayerController>(Player->GetController()));
      }*/
}

void ABlastRadiusGameMode::BeginPlay()
{
    Super::BeginPlay();

    GetWorld()->GetTimerManager().SetTimer(StartCountDown, this, &ABlastRadiusGameMode::EnablePlayers, 3.0f, false);
}

void ABlastRadiusGameMode::EnablePlayers()
{
    TArray<AActor*> Players;
    ABlastRadiusCharacter* Player = nullptr;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABlastRadiusCharacter::StaticClass(), Players);

    for (AActor* Actor : Players)
    {
        Player = Cast<ABlastRadiusCharacter>(Actor);
        Player->GetController()->EnableInput(Cast<APlayerController>(Player->GetController()));
    }

}

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

        NewPlayer->DisableInput(Cast<APlayerController>(character->GetController()));
    }
}

