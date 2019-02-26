// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class ABlastRadiusProjectile;


#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlastRadiusWeapon.generated.h"

UCLASS()
class BLASTRADIUS_API ABlastRadiusWeapon : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ABlastRadiusWeapon();

    FVector MuzzleDirectionOffSet;

    FRotator MuzzleRotation;

    FVector MuzzleLocation;

    float MagnitudeOffSet;
    /** Projectile **/
    //Projectile template to fire.
    UPROPERTY(EditDefaultsOnly, Category = Projectile)
        TSubclassOf<class ABlastRadiusProjectile> ProjectileClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
        class UStaticMeshComponent* MeshComponent;

    UPROPERTY(EditAnywhere, Category = Muzzle, meta = (AllowPrivateAccess = "true"))
        class UArrowComponent* MuzzleArrow;
protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    void Fire();

    void Attach(class ABlastRadiusCharacter* Character);
};
