// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlastRadiusPickup.generated.h"

UCLASS()
class BLASTRADIUS_API ABlastRadiusPickup : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ABlastRadiusPickup();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

#pragma region Components
public:
    /* Audio Component */
    UPROPERTY(EditAnywhere, Category = "Audio", meta = (AllowPrivateAccess = "true"))
        class UAudioComponent* AudioComponent;

    /* Pickup Sound */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
        class USoundBase* PickupSound;

    /* Sphere Component (Collision) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision)
        class USphereComponent* SphereComponent;

    /* Mesh Component */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
        class UStaticMeshComponent* MeshComponent;

    /* Particle System Component */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FX)
        class UParticleSystemComponent* ParticleSystemComponent;

    /* Pickup Particle FX */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FX)
        UParticleSystem* PickupFX;
#pragma endregion Components

#pragma region Members
protected:
    /* Defines time before the pickup 'respawns' in world. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Respawn)
        float RespawnTime = 30.0f;
#pragma endregion Members

#pragma region Methods
public:
    /* Show in world and enable interaction. */
    UFUNCTION(NetMulticast, Reliable)
        virtual void Enable();

    /* Hide in world and prevent interaction. */
    UFUNCTION(NetMulticast, Reliable)
        virtual void Disable();

    /* On component overlap. */
    UFUNCTION()
        void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
#pragma endregion Methods
};
