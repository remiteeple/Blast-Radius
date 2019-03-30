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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Respawn)
        float RespawnTime = 30.0f;

public:	
	// Sets default values for this actor's properties
	ABlastRadiusPickup();

    // Called every frame
    virtual void Tick(float DeltaTime) override;
    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

    UFUNCTION(NetMulticast, Reliable)
        virtual void Disable();

    UFUNCTION(NetMulticast, Reliable)
        virtual void Enable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
    virtual void PostInitializeComponents() override;

};
