// Fill out your copyright notice in the Description page of Project Settings.

#include "BlastRadiusPickup.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Character/AIBlastRadiusCharacter.h"

// Sets default values
ABlastRadiusPickup::ABlastRadiusPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    /* Setup Sphere Component (Collision) */
    SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
    SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    //SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    //SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ABlastRadiusPickup::OnOverlapBegin);
    RootComponent = SphereComponent;

    /* Setup Mesh Component */
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MeshComponent->SetupAttachment(RootComponent);

    /* Setup Particle System Component */
    ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Pickup Particle System Component"));
    ParticleSystemComponent->SetupAttachment(RootComponent);

    /* Setup Pickup Particles */
    PickupFX = CreateDefaultSubobject<UParticleSystem>(TEXT("Pickup Particles"));

    /* Setup Audio Component */
    AudioComponent = CreateDefaultSubobject<UAudioComponent>("Pickup Sound");
    AudioComponent->bAutoActivate = false;
    AudioComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
    AudioComponent->SetupAttachment(RootComponent);

    // Enable network replication.
    SetReplicates(true);
    SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ABlastRadiusPickup::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABlastRadiusPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABlastRadiusPickup::Disable_Implementation()
{
    if (SphereComponent)
    {
        /* Hide & Disable Pickup */
        MeshComponent->SetHiddenInGame(true);
        SphereComponent->SetActive(false);
        SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        // Re-enable the health pickup after specific time passes.
        FTimerHandle RespawnTimer;
        GetWorldTimerManager().SetTimer(RespawnTimer, this, &ABlastRadiusPickup::Enable, RespawnTime, false);
    }
}

void ABlastRadiusPickup::Enable_Implementation()
{
    if (SphereComponent)
    {
        /* Show & Enable Pickup */
        MeshComponent->SetHiddenInGame(false);
        SphereComponent->SetActive(true);
        SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    }
}

void ABlastRadiusPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    ABlastRadiusCharacter* Character = Cast<ABlastRadiusCharacter>(OtherActor);
    if (Character)
    {
        // Play particle animation.
        if (PickupFX)
        {
            ParticleSystemComponent->SecondsBeforeInactive = 0.5;
            UGameplayStatics::SpawnEmitterAtLocation(this, PickupFX, GetActorLocation());
            ParticleSystemComponent->SetTemplate(PickupFX);
        }

        // Play the audio for pickup.
        if (PickupSound)
        {
            AudioComponent->SetSound(PickupSound);
            AudioComponent->Play();
        }
    }
}