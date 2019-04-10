// Fill out your copyright notice in the Description page of Project Settings.

#include "BlastRadiusBlastzoneBox.h"
#include "Components/BoxComponent.h"
#include "Gameplay/BlastRadiusPlayerState.h"
#include "Character/AIBlastRadiusCharacter.h"

// Sets default values
ABlastRadiusBlastzoneBox::ABlastRadiusBlastzoneBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    KillBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Kill Capsule"));
    KillBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    KillBox->OnComponentBeginOverlap.AddDynamic(this, &ABlastRadiusBlastzoneBox::OnComponentOverlapBegin);
}

// Called when the game starts or when spawned
void ABlastRadiusBlastzoneBox::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABlastRadiusBlastzoneBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABlastRadiusBlastzoneBox::OnComponentOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (Role = ROLE_Authority)
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

