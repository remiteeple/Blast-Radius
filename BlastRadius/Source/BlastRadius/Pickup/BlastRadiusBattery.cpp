// Fill out your copyright notice in the Description page of Project Settings.

#include "BlastRadiusBattery.h"


ABlastRadiusBattery::ABlastRadiusBattery()
{
    Tags.Add("Battery");
}

void ABlastRadiusBattery::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);
}
