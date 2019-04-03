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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision)
        class USphereComponent* SphereComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
        class UStaticMeshComponent* MeshComponent;
#pragma region ParticleFX
    /* Defines time before the pickup 'respawns' in world. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Respawn)
        float RespawnTime = 30.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FX)
        class UParticleSystemComponent* ParticleSystemComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FX)
        UParticleSystem* PickupFX;
#pragma endregion ParticleFX

#pragma region SoundFX
    UPROPERTY(EditAnywhere, Category = "Audio", meta = (AllowPrivateAccess = "true"))
        class UAudioComponent* AudioComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
        class USoundBase* PickupSound;
#pragma endregion SoundFX

public:
    // Sets default values for this actor's properties
    ABlastRadiusPickup();

    // Called every frame
    virtual void Tick(float DeltaTime) override;
    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

    /* Show in world and enable interaction. */
    UFUNCTION(NetMulticast, Reliable)
        virtual void Enable();

    /* Hide in world and prevent interaction. */
    UFUNCTION(NetMulticast, Reliable)
        virtual void Disable();

    /* On component overlap. */
    UFUNCTION()
        void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    virtual void PostInitializeComponents() override;
};
