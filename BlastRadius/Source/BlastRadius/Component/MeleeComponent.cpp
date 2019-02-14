        // Fill out your copyright notice in the Description page of Project Settings.

#include "MeleeComponent.h"
#include "Character/BlastRadiusCharacter.h"

// Sets default values for this component's properties
UMeleeComponent::UMeleeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMeleeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UMeleeComponent::OnActorHit(AActor* Self, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor != nullptr)
    {
        if (OtherActor != Self)
        {
            ABlastRadiusCharacter* OtherCharacter = Cast<ABlastRadiusCharacter>(OtherActor);
            if (OtherCharacter != nullptr)
            {
                FDamageEvent DamageEvent;
                OtherCharacter->TakeDamage(Damage, DamageEvent, nullptr, GetOwner());
            }
            else
                return;
        }
    }
}

// Called every frame
void UMeleeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// did we hit other player?
    // if true apply damage
}

void UMeleeComponent::Melee()
{
    // handle the animation
}