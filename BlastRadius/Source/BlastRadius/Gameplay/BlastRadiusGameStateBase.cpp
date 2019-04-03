// Fill out your copyright notice in the Description page of Project Settings.

#include "BlastRadiusGameStateBase.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void ABlastRadiusGameStateBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    //REPLICATE 
    DOREPLIFETIME(ABlastRadiusGameStateBase, TeamsEnabled);
    DOREPLIFETIME(ABlastRadiusGameStateBase, TeamOneScore);
    DOREPLIFETIME(ABlastRadiusGameStateBase, TeamTwoScore);

    DOREPLIFETIME(ABlastRadiusGameStateBase, TeamOneMatchScore);
    DOREPLIFETIME(ABlastRadiusGameStateBase, TeamTwoMatchScore);

    DOREPLIFETIME(ABlastRadiusGameStateBase, TeamOneSize);
    DOREPLIFETIME(ABlastRadiusGameStateBase, TeamTwoSize);
    DOREPLIFETIME(ABlastRadiusGameStateBase, GameTime);

    DOREPLIFETIME(ABlastRadiusGameStateBase, TeamOnePMaterials);
    DOREPLIFETIME(ABlastRadiusGameStateBase, TeamTwoPMaterials);

}
