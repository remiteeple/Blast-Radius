// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystem.h"
#include "BlastRadiusProjectile.generated.h"

UCLASS()
class BLASTRADIUS_API ABlastRadiusProjectile : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ABlastRadiusProjectile();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

#pragma region Methods
public:
    /* Apply direction to projectile movement component */
    void FireInDirection(const FVector& ShootDirection);

    /* Flip the velocity of the projectile */
    void FlipVelocity();

    /* Timer callable function for destruction */
    void DestroySelf();

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
        class UStaticMeshComponent* ProjectileMeshComponent;

    /* Projectile movement component */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
        class UProjectileMovementComponent* ProjectileMovementComponent;

    /* Particle System Component */
    UPROPERTY(EditDefaultsOnly, Category = FX)
        class UParticleSystemComponent* ParticleSystemComponent;

    /* Projectile Particle System */
    UPROPERTY(EditDefaultsOnly, Category = FX)
        UParticleSystem* ProjectileFX;

    /* Projectile Destruction Particle System */
    UPROPERTY(EditDefaultsOnly, Category = FX)
        UParticleSystem* ProjectileDestroyFX;
#pragma endregion Components

#pragma region Members
public:
    /* Projectile Variables */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
        float LifeSpan;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
        float LaserDamage;
    UPROPERTY(BlueprintReadWrite, Category = Projectile)
        int BouncesMade;
    UPROPERTY(BlueprintReadWrite, Category = Projectile)
        float KnockbackFactor;
    UPROPERTY(BlueprintReadWrite, Category = Projectile)
        float BlowBackRange;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
        int MaxBounces;
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
    FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovementComponent; }
#pragma endregion Getters
};
