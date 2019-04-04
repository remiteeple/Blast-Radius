// Fill out your copyright notice in the Description page of Project Settings.

#include "BlastRadiusExplosion.h"
#include "Runtime/Engine/Public/WorldCollision.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"
#include "Components/SkeletalMeshComponent.h"
#include "Character/BlastRadiusCharacter.h"
#include "Component/HealthComponent.h"
#include "Runtime/Engine/Classes/GameFramework/DamageType.h"


// Sets default values
ABlastRadiusExplosion::ABlastRadiusExplosion()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    ExplosionRadius = 400;
    Damage = 25;

    //change to false later. To be toggled via blueprint editor.
    DebugSphereEnabled = true;
}

// Called when the game starts or when spawned
void ABlastRadiusExplosion::BeginPlay()
{
    Super::BeginPlay();

    //array of hit, fill this later with colliding actors using GetWorld()->SweepByMultiChannel();
    TArray<FHitResult> HitResults;

    //store the explosion location
    FVector ExplosionLocation = GetActorLocation();

    // Create Collision with FCollisionShape::MakeSphere();
    FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(ExplosionRadius);

    //drawDebug sphere for testing
    if (DebugSphereEnabled)
    {
        DrawDebugSphere(GetWorld(), ExplosionLocation, ExplosionRadius, 50, FColor::Green, true, 5);
    }

    bool Ishit = GetWorld()->SweepMultiByChannel(HitResults, ExplosionLocation, ExplosionLocation, FQuat::Identity, ECC_Pawn, CollisionSphere);

    if (Ishit)
    {
        for (auto Hit : HitResults)
        {
            //GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Blue, Hit.GetActor()->GetActorLabel());

            FVector difference =  Hit.GetActor()->GetActorLocation() - ExplosionLocation;
            FVector direction = difference.GetSafeNormal();
            float distance = difference.Size();

            FVector BlowBackVector = direction * 10000 ;

            if (Hit.GetActor()->ActorHasTag("Player"))
            {
                ABlastRadiusCharacter* Character = Cast<ABlastRadiusCharacter>(Hit.GetActor());

                const UDamageType* Laser_DamageType = Cast<UDamageType>(UDamageType::StaticClass());
                Character->GetHealthComponent()->TakeDamage(Damage, Laser_DamageType, Character->GetInstigatorController(), GetOwner(), BlowBackVector );
            }
        }
    }
}

// Called every frame
void ABlastRadiusExplosion::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

