// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//class UInputComponent;

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystem.h"
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

    UPROPERTY()
        int m_MaxBounceAmount;

    UPROPERTY()
        float m_KnockbackFactor;

    UPROPERTY(EditDefaultsOnly, Category = ProjectileDamage)
        TSubclassOf<UDamageType> m_DamageType;

    /** spawn timer  */
    UPROPERTY(BlueprintReadOnly,
        Category = "Config",
        meta = (AllowPrivateAccess = "true"))
        FTimerHandle SpawnTimer;

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
        UParticleSystem* ProjectileFX;


    UPROPERTY(VisibleAnywhere)
        class UParticleSystemComponent* PSC; 

public:
    // Sets default values for this actor's properties
    ABlastRadiusProjectile();

    UFUNCTION()
        void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    //void ABlastRadiusSword::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)


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

    void DestroySelf();
};
