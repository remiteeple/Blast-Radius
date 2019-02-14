// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//class UInputComponent;

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "BlastRadiusProjectile.generated.h"



UCLASS()
class BLASTRADIUS_API ABlastRadiusProjectile : public AActor
{
    GENERATED_BODY()

        UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
        class USphereComponent* CollisionComp;

    UPROPERTY(VisibleAnywhere, Category = "Mesh")
        class UStaticMeshComponent* ProjectileMeshComp;

    /** Projectile movement component */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
        class UProjectileMovementComponent* ProjectileMovementComp;

    UPROPERTY()
        float m_LifeSpan;

    UPROPERTY()
        float m_LaserDamage;

public:
    // Sets default values for this actor's properties
    ABlastRadiusProjectile();

    UFUNCTION()
        void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    /** Returns CollisionComp subobject **/
    FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }
    /** Returns ProjectileMovement subobject **/
    FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovementComp; }

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;



public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    void FireInDirection(const FVector& ShootDirection);
};
