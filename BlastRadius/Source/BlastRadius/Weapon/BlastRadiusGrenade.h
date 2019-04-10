// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystem.h"
#include "BlastRadiusGrenade.generated.h"

UCLASS()
class BLASTRADIUS_API ABlastRadiusGrenade : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ABlastRadiusGrenade();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

#pragma region Methods
public:
    /* Apply direction to projectile movement component */
    void LobInDirection(const FVector& ShootDirection);

    /* Spawn explosion */
    void Explode();

    /* Timer callable function for destruction */
    void DestroySelf();

    /* Collision Response */
    UFUNCTION()
        void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

#pragma endregion Methods

#pragma region Components
protected:
    /* Sphere Component (Collision) */
    UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
        class USphereComponent* CollisionComponent;

    /* Static Mesh Component */
    UPROPERTY(VisibleAnywhere, Category = Mesh)
        class UStaticMeshComponent* GrenadeMeshComponent;

    /* Projectile movement component */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
        class UProjectileMovementComponent* GrenadeMovementComponent;

    /* Audio Component */
    UPROPERTY(EditAnywhere, Category = Audio, meta = (AllowPrivateAccess = "true"))
        class UAudioComponent* AudioComponent;

    /* Shooting Sound */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
        class USoundBase* ExplodeSound;

    /* Particle System Component */
    UPROPERTY(EditDefaultsOnly, Category = FX)
        class UParticleSystemComponent* ParticleSystemComponent;

    /* Projectile Particle System */
    UPROPERTY(EditDefaultsOnly, Category = FX)
        UParticleSystem* GrenadeFX;

    /* Projectile Destruction Particle System */
    UPROPERTY(EditDefaultsOnly, Category = FX)
        UParticleSystem* GrenadeDestroyFX;
#pragma endregion Components

#pragma region Members
public:
    /* Grenade Variables */
    UPROPERTY(BlueprintReadWrite, Category = Projectile)
        float FuseTime;
    UPROPERTY(BlueprintReadWrite, Category = Projectile)
        float GrenadeDamage;
    UPROPERTY(BlueprintReadWrite, Category = Projectile)
        float KnockbackFactor;
    UPROPERTY(BlueprintReadWrite, Category = Projectile)
        float BlowBackRange;

    /* Explosion Template */
    UPROPERTY(EditDefaultsOnly, Category = Projectile)
        TSubclassOf<class ABlastRadiusExplosion> ExplosionClass;

    /* Damage Identifier */
    UPROPERTY(EditDefaultsOnly, Category = ProjectileDamage)
        TSubclassOf<UDamageType> DamageType;

    /* Spawn Timer */
    UPROPERTY(BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
        FTimerHandle SpawnTimer;
#pragma endregion Members

#pragma region Getters
public:
    /* Returns CollisionComp subobject */
    FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComponent; }

    /* Returns ProjectileMovement subobject */
    FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return GrenadeMovementComponent; }
#pragma endregion Getters
};
