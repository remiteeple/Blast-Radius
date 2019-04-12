// Fill out your copyright notice in the Description page of Project Settings.

#include "BlastRadiusBlastzoneSphere.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"

#include "Character/BlastRadiusCharacter.h"
#include "BlastRadius/Gameplay/BlastRadiusPlayerState.h"

// Sets default values
ABlastRadiusBlastzoneSphere::ABlastRadiusBlastzoneSphere()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    OuterKillSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Kill Shere"));
    OuterKillSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    OuterKillSphere->OnComponentEndOverlap.AddDynamic(this, &ABlastRadiusBlastzoneSphere::OnComponentEndOverlap);
}

// Called when the game starts or when spawned
void ABlastRadiusBlastzoneSphere::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void ABlastRadiusBlastzoneSphere::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ABlastRadiusBlastzoneSphere::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (Role == ROLE_Authority)
    {
        if (OtherActor)
        {
            ABlastRadiusCharacter* Player = Cast<ABlastRadiusCharacter>(OtherActor);
            if (Player != nullptr)
            {
                if (Player->bIsDead == false)
                {
                    ABlastRadiusPlayerState* PlayerState = Player->GetPlayerState();
                    if (PlayerState)
                    {
                        PlayerState->DecrementLives();
                        Player->OnDeath();
                    }
                }
            }
        }
    }
}