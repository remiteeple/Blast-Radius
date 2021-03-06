// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlastRadiusBlastzoneSphere.generated.h"

UCLASS()
class BLASTRADIUS_API ABlastRadiusBlastzoneSphere : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
    ABlastRadiusBlastzoneSphere();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay)
        class USphereComponent* OuterKillSphere;

    UPROPERTY(EditAnywhere)
        class UStaticMeshComponent* SphereMesh;

    UFUNCTION()
        void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};