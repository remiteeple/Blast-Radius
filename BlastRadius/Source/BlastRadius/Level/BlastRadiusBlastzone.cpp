// Fill out your copyright notice in the Description page of Project Settings.

#include "BlastRadiusBlastzone.h"
#include "Components/SphereComponent.h"
#include "Character/BlastRadiusCharacter.h"
#include "BlastRadius/Gameplay/BlastRadiusPlayerState.h"
#include "BlastRadius/Gameplay/BlastRadiusGameStateBase.h"
#include "Runtime/Engine/Classes/Engine/World.h"

// Sets default values
ABlastRadiusBlastzone::ABlastRadiusBlastzone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    
    Blastzone = CreateDefaultSubobject<USphereComponent>(TEXT("Blastzone"));
    Blastzone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    
    Blastzone->OnComponentBeginOverlap.AddDynamic(this, &ABlastRadiusBlastzone::OnComponentBeginOverlap);
}

// Called when the game starts or when spawned
void ABlastRadiusBlastzone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABlastRadiusBlastzone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABlastRadiusBlastzone::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if(OtherActor)
    {
            ABlastRadiusCharacter* Player = Cast<ABlastRadiusCharacter>(OtherActor);
            if (Player != nullptr)
            {
                APlayerState* PlayerState =  Player->GetInstigatorController()->PlayerState;
                Cast<ABlastRadiusPlayerState>(PlayerState)->DecrementLives();
            }
    }
}

