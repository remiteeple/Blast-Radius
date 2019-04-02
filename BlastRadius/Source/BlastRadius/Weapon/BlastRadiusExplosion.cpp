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

    ExplosionRadius = 500;

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

    bool Ishit = GetWorld()->SweepMultiByChannel(HitResults, ExplosionLocation, ExplosionLocation, FQuat::Identity, ECC_WorldStatic, CollisionSphere);

    if (Ishit)
    {
        for (auto Hit : HitResults)
        {
            GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Blue, Hit.GetActor()->GetActorLabel());

            FVector difference = ExplosionLocation - Hit.GetActor()->GetActorLocation();
            FVector direction = difference.GetSafeNormal();
            float distance = difference.Size();

            // Calculate direction & distance to impact for each character(actor)
            FVector directionToImpactLocation = FVector(difference - Hit.GetActor()->GetActorLocation()).GetSafeNormal();
            float playerToImpactDistance = FVector(ExplosionLocation - Hit.GetActor()->GetActorLocation()).Size();

            FVector BlowBackVector = directionToImpactLocation * 1000;

            if (Cast<ABlastRadiusCharacter>(Hit.GetActor()) != nullptr)
            {
                ABlastRadiusCharacter* Character = Cast<ABlastRadiusCharacter>(Hit.GetActor());

                const UDamageType* Laser_DamageType = Cast<UDamageType>(UDamageType::StaticClass());
                Character->GetHealthComponent()->TakeDamage(100, Laser_DamageType, Character->GetInstigatorController(), GetOwner(), BlowBackVector * GetVelocity());
                //GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "BlowBackDamage % - " + FString::SanitizeFloat(Character->GetHealthComponent()->GetCurrentHealth())); // DEBUG

                USkeletalMeshComponent* MeshComp = Character->GetSkeletalMesh();

                if (MeshComp != nullptr)
                {
                    MeshComp->AddRadialImpulse(ExplosionLocation, ExplosionRadius, 1000.0f, ERadialImpulseFalloff::RIF_Linear);


                }
            }
        }
    }
}

// Called every frame
void ABlastRadiusExplosion::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

