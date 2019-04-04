// Fill out your copyright notice in the Description page of Project Settings.

#include "BlastRadiusSword.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Character/BlastRadiusCharacter.h"
#include "Component/HealthComponent.h"
#include "Weapon/BlastRadiusProjectile.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ABlastRadiusSword::ABlastRadiusSword()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    //StaticMesh->SetupAttachment(RootComponent);
    //StaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RootComponent = StaticMesh;

    HitBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Hit Box"));
    HitBoxComponent->SetupAttachment(RootComponent);
    //SetActorEnableCollision(false);
    HitBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SetActorHiddenInGame(true);
    HitBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ABlastRadiusSword::OnOverlapBegin);
}

// Called when the game starts or when spawned
void ABlastRadiusSword::BeginPlay()
{
    Super::BeginPlay();


}

// Called every frame
void ABlastRadiusSword::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);


}

void ABlastRadiusSword::Attach(class ABlastRadiusCharacter* Character)
{
    verify(Character != nullptr && "Weapon's attach function called with null character!");

    /* Set owner of this weapon.*/
    SetOwner(Character);
    SetActorHiddenInGame(true);

    // Disable weapon's physics.
    //SetActorEnableCollision(false);

    // Attach weapon to the character's mesh.
    AttachToComponent(Character->GetSkeletalMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "SwordSocket");

}

void ABlastRadiusSword::Activate()
{
    SetActorHiddenInGame(false);
}

void ABlastRadiusSword::PutAway()
{
    SetActorHiddenInGame(true);
}

ABlastRadiusSword* ABlastRadiusSword::GetSword()
{
    return this;
}

void ABlastRadiusSword::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ABlastRadiusProjectile* OtherProjectile = Cast<ABlastRadiusProjectile>(OtherActor);
    if (OtherProjectile != nullptr)
    {
        OtherProjectile->FlipVelocity();

    }
}

