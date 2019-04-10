// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlastRadiusExplosion.generated.h"

UCLASS()
class BLASTRADIUS_API ABlastRadiusExplosion : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ABlastRadiusExplosion();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

#pragma region Members
    UPROPERTY(EditAnywhere, Category = "Explosion")
        float ExplosionRadius;

    UPROPERTY(EditAnywhere, Category = "DebugSphere")
        bool DebugSphereEnabled;

    UPROPERTY(EditAnywhere, Category = "Explosion")
        float Damage;
#pragma region Members
};
