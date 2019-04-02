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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, Category = "Explosion")
    float ExplosionRadius;

    UPROPERTY(EditAnywhere, Category = "DebugSphere")
        bool DebugSphereEnabled;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
