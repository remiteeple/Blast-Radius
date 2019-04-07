// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BlinkComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTRADIUS_API UBlinkComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBlinkComponent();

    /* Blink the owner. */
    UFUNCTION()
        void Blink();

private:
    /* Audio Component */
    UPROPERTY(EditDefaultsOnly, Category = Audio)
        class UAudioComponent* AudioComponent;

    /* Blink Sound */
    UPROPERTY(EditDefaultsOnly, Category = Audio)
        class USoundBase* BlinkSound;

    /* Particle System Component */
    UPROPERTY(EditDefaultsOnly, Category = FX)
        class UParticleSystemComponent* ParticleSystemComponent;

    /* Blink Particles */
    UPROPERTY(EditDefaultsOnly, Category = FX)
        UParticleSystem* BlinkParticleFX;
};
