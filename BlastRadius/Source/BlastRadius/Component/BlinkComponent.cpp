// Fill out your copyright notice in the Description page of Project Settings.

#include "BlinkComponent.h"
#include "BlastRadius/Character/BlastRadiusCharacter.h"
#include "GameFramework/Controller.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"


UBlinkComponent::UBlinkComponent()
{
    // Setup the Audio Component
    AudioComponent = CreateDefaultSubobject<UAudioComponent>("BlinkSound");
    AudioComponent->bAutoActivate = false;

    // Setup the Particle System Component
    ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Blink Particle System"));

    // Setup the Blink Particle
    BlinkParticleFX = CreateDefaultSubobject<UParticleSystem>(TEXT("Blink Particle FX"));
}

void UBlinkComponent::Blink()
{
        ABlastRadiusCharacter* Player = Cast<ABlastRadiusCharacter>(GetOwner());

        // Calculate start position and end position.
        FVector StartLocation = Player->GetActorLocation();
        FVector Direction = Player->GetActorRotation().Vector();
        FVector EndLocation = StartLocation + Direction * BlinkDistance;

        // Spawn particle before translating player position.
        if (BlinkParticleFX)
        {
            ParticleSystemComponent->SetRelativeLocation(Player->GetActorLocation());
            ParticleSystemComponent->SecondsBeforeInactive = 0.5;
            UGameplayStatics::SpawnEmitterAtLocation(this, BlinkParticleFX, Player->GetActorLocation());
            ParticleSystemComponent->SetTemplate(BlinkParticleFX);
        }

        //Play the audio for blinking.
        AudioComponent->SetSound(BlinkSound);
        AudioComponent->Play();

        // Move player to end location.
        Player->SetActorLocation(FMath::Lerp(StartLocation, EndLocation, 1));

        // Spawn Beam
        if (BlinkBeamParticleFX)
        {
            ParticleSystemComponent->SetTemplate(BlinkBeamParticleFX);
            ParticleSystemComponent->SetBeamSourcePoint(1, StartLocation, 0);
            ParticleSystemComponent->SetBeamTargetPoint(1, EndLocation, 0);
        }

        // Spawn particle after translating player position.
        if (BlinkParticleFX)
        {
            ParticleSystemComponent->SetTemplate(BlinkParticleFX);
            ParticleSystemComponent->SetRelativeLocation(Player->GetActorLocation());
            UGameplayStatics::SpawnEmitterAtLocation(this, BlinkParticleFX, Player->GetActorLocation());
        }
}