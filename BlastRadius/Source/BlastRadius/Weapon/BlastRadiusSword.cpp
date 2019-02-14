// Fill out your copyright notice in the Description page of Project Settings.

#include "BlastRadiusSword.h"


// Sets default values
ABlastRadiusSword::ABlastRadiusSword()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABlastRadiusSword::BeginPlay()
{
	Super::BeginPlay();
    /*void UMeleeComponent::OnActorHit(AActor* Self, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
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
    }*/
	
}

// Called every frame
void ABlastRadiusSword::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

