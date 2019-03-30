// Fill out your copyright notice in the Description page of Project Settings.

#include "BlastRadiusPickup.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ABlastRadiusPickup::ABlastRadiusPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Enable network replication.
    SetReplicates(true);
    SetReplicateMovement(true);

    SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
    SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    //SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    //SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    RootComponent = SphereComponent;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MeshComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABlastRadiusPickup::BeginPlay()
{
	Super::BeginPlay();

    // Re-enable the health pickup after specific time passes.
    FTimerHandle RespawnTimer;
    GetWorldTimerManager().SetTimer(RespawnTimer, this, &ABlastRadiusPickup::Enable, RespawnTime, true);
}

// Called every frame
void ABlastRadiusPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABlastRadiusPickup::PostInitializeComponents()
{
    Super::PostInitializeComponents();

}

void ABlastRadiusPickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);
}


void ABlastRadiusPickup::Disable_Implementation()
{
    if (SphereComponent)
    {
        this->SetActorHiddenInGame(true);
        SphereComponent->SetActive(false);
        SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}

void ABlastRadiusPickup::Enable_Implementation()
{
    if (SphereComponent)
    {
        this->SetActorHiddenInGame(false);
        SphereComponent->SetActive(true);
        SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    }
}