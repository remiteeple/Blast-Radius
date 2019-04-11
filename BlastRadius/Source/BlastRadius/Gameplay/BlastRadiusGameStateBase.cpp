// Fill out your copyright notice in the Description page of Project Settings.

#include "BlastRadiusGameStateBase.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Character/BlastRadiusCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "BlastRadiusPlayerState.h"

void ABlastRadiusGameStateBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    //REPLICATE 
    DOREPLIFETIME(ABlastRadiusGameStateBase, TeamsEnabled);
    DOREPLIFETIME(ABlastRadiusGameStateBase, WinningTeam);
    DOREPLIFETIME(ABlastRadiusGameStateBase, LosingTeam);

    DOREPLIFETIME(ABlastRadiusGameStateBase, TeamOneSize);
    DOREPLIFETIME(ABlastRadiusGameStateBase, TeamTwoSize);

    DOREPLIFETIME(ABlastRadiusGameStateBase, TeamOnePMaterials);
    DOREPLIFETIME(ABlastRadiusGameStateBase, TeamTwoPMaterials);
}

void ABlastRadiusGameStateBase::StartRound()
{
    if (Role > ROLE_SimulatedProxy)
    {
        TArray<AActor*> Players;
        ABlastRadiusCharacter* Player = nullptr;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABlastRadiusCharacter::StaticClass(), Players);

        /* Enable input for all players spawned in world */
        for (AActor* Actor : Players)
        {
            
                Player = Cast<ABlastRadiusCharacter>(Actor);
                if (Player->GetInstigatorController() != nullptr)
                    Player->GetController()->DisableInput(Cast<APlayerController>(Player->GetController()));
        }
    }
    NM_StartRound_Implementation();
}

void ABlastRadiusGameStateBase::BeginPlay()
{
    /* Call function to disable players*/
    StartRound();
    /* Start countdown before starting gameplay to enable characters */
    GetWorld()->GetTimerManager().SetTimer(StartCountDown, this, &ABlastRadiusGameStateBase::EnablePlayers, 3.0f, false);
}

void ABlastRadiusGameStateBase::EnablePlayers()
{
    if (Role > ROLE_SimulatedProxy)
    {
        /* Add all BlastRadiusCharacters To An Array*/
        TArray<AActor*> Players;
        ABlastRadiusCharacter* Player = nullptr;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABlastRadiusCharacter::StaticClass(), Players);

        /* Enable input for all players spawned in world */
        for (AActor* Actor : Players)
        {
            
                Player = Cast<ABlastRadiusCharacter>(Actor);
                if (Player->GetInstigatorController() != nullptr)
                    Player->GetController()->EnableInput(Cast<APlayerController>(Player->GetController()));
            
        }
    }

    NM_EnablePlayers_Implementation();
}

void ABlastRadiusGameStateBase::NM_EnablePlayers_Implementation()
{
    if (Role <= ROLE_SimulatedProxy)
    {
        /* Get list of all players in world */
        TArray<AActor*> Players;
        ABlastRadiusCharacter* Player = nullptr;
        /* Add all BlastRadiusCharacters To An Array*/
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABlastRadiusCharacter::StaticClass(), Players);

        /* Enable input for all players spawned in world */
        for (AActor* Actor : Players)
        {
            Player = Cast<ABlastRadiusCharacter>(Actor);
            if(Player->GetInstigatorController() != nullptr)
            Player->GetController()->EnableInput(Cast<APlayerController>(Player->GetController()));
        }
    }
}

bool ABlastRadiusGameStateBase::NM_EnablePlayers_Validate()
{
    return true;
}

void ABlastRadiusGameStateBase::NM_StartRound_Implementation()
{
    if (Role <= ROLE_SimulatedProxy)
    {
        /* Get list of all players in world */
        TArray<AActor*> Players;
        ABlastRadiusCharacter* Player = nullptr;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABlastRadiusCharacter::StaticClass(), Players);

        /* Disable input for all players spawned in world */
        for (AActor* Actor : Players)
        {
            Player = Cast<ABlastRadiusCharacter>(Actor);
            if(Player->GetController() != nullptr)
            Player->GetController()->DisableInput(Cast<APlayerController>(Player->GetController()));
        }
    }
}

bool ABlastRadiusGameStateBase::NM_StartRound_Validate()
{
    return true;
}
