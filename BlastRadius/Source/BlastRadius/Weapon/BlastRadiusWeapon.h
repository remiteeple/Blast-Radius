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

    // Called every frame
    virtual void Tick(float DeltaTime) override;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

#pragma region Members
public:

    /** Projectile Template **/
    UPROPERTY(EditDefaultsOnly, Category = Projectile)
        TSubclassOf<class ABlastRadiusProjectile> ProjectileClass;
#pragma endregion Members

#pragma region Methods
public:
    /* Fire the weapon */
    UFUNCTION()
        void Fire();

    /* Server Call for Shooting */
    UFUNCTION(NetMulticast, Reliable, WithValidation)
        void NetMultiCastFire();

    /* Attach the weapon to the character */
    void Attach(class ABlastRadiusCharacter* Character);
#pragma endregion Methods

#pragma region Components
private:
    /* Static Mesh Component */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
        class UStaticMeshComponent* MeshComponent;

    /* Arrow Component */
    UPROPERTY(EditAnywhere, Category = Muzzle, meta = (AllowPrivateAccess = "true"))
        class UArrowComponent* MuzzleArrow;

    /* Audio Component */
    UPROPERTY(EditAnywhere, Category = "Audio", meta = (AllowPrivateAccess = "true"))
        class UAudioComponent* AudioComponent;

    /* Shooting Sound */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
        class USoundBase* ShootingSound;

    /* Particle System Component */
    UPROPERTY(VisibleAnywhere)
        class UParticleSystemComponent* ParticleSystemComponent;

    /* Particle System */
    UPROPERTY(EditDefaultsOnly, Category = "Effects")
        UParticleSystem* ProjectileFX;
#pragma endregion Components
};
