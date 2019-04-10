// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlastRadiusBlastzoneBox.generated.h"

UCLASS()
class BLASTRADIUS_API ABlastRadiusBlastzoneBox : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ABlastRadiusBlastzoneBox();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay)
        class UBoxComponent* KillBox;

    UFUNCTION()
        void OnComponentOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
