// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlastRadiusCharacter.generated.h"

UCLASS(config=Game)
class ABlastRadiusCharacter : public ACharacter
{
	GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
        class UHealthComponent* HealthComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
        class UEnergyComponent* EnergyComponent;

protected:
	ABlastRadiusCharacter();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
	/** Character movement variable **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
        float MaxWalkSpeed;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
        float MaxRunSpeed;
    /** Spawn location variable **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
        FVector SpawnPoint;
    /** Spawn timer **/
    FTimerHandle SpawnTimer;
    /** Spawn Delay**/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
        float SpawnDelay;
    /** Animations **/
    UPROPERTY(EditDefaultsOnly)
        UAnimMontage* HipFireAnimation;
    UPROPERTY(EditDefaultsOnly)
        UAnimMontage* AimFireAnimaion;
    UPROPERTY(EditDefaultsOnly)
        UAnimMontage* MeleeAttackAnimation;
    UPROPERTY(EditDefaultsOnly)
        FTimerHandle MeleeTimer;

    /** Porjectile **/
    //Goes in weapon, in Character for testing. Used to define projectile spawn point.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
        FVector MuzzleOffset;
    //Projectile template to fire.
    UPROPERTY(EditDefaultsOnly, Category = Projectile)
        TSubclassOf<class ABlastRadiusProjectile> ProjectileClass;
    //Sword Template to use
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
        TSubclassOf<class ABlastRadiusSword> SwordClass;
        ABlastRadiusSword* Sword;
    /** Called when actor hit **/
    void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    /** Called for directional movement */
    void Move(FVector Direction, float Scale);

    /** Called for directional rotation of character  **/
    void LookAt(FVector Direction);

    /** Called when blink is activated **/
    void Blink();

    /** Called for aiming **/
    void Aim(bool Toggle);

    /** Called for shooting **/
    void Shoot();

    /** Call for shooting **/
    void Fire();

    /** Called for melee attack **/
    void Melee();

    /** Called to hide sword after melee **/
    void PutAwaySword();

    /* Called when player health passes lower limit */
    UFUNCTION()
        void OnDeath();

    /** Teleports player upon stock loss to spawm point **/
    void Respawn();
public:
	/** State Definitions **/
	// Default false
	bool bIsWalking = false;
	bool bIsAiming = false;
    bool bIsMeleeAttacking = false;
	bool bIsFiring = false;
	bool bIsBlinking = false;

private:
	/** Component Declarations **/
	//class UHealthComponent* HealthComponent; Commented out as it has an attached component, now.
	//class UEnergyComponent* EnergyComponent; Commented out as it has an attached component, now.

	    class USkeletalMeshComponent* SkeletalMesh;

	    class UBlinkComponent* BlinkComponent;

        class UMeleeComponent* MeleeComponent;

	    class UCharacterAnimInstance* AnimationInstance;

	/** Weapon the character uses **/
	//UPROPERTY()
	//    class AWeaponBase* Weapon; // Weapon

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return TopDownCamera; }

	/** Returns SkeletalMesh component **/
	FORCEINLINE class USkeletalMeshComponent* GetSkeletalMesh() const { return SkeletalMesh; }

    /** Returns Health component **/
    FORCEINLINE class UHealthComponent* GetHealthComponent() const { return HealthComponent; }

    /** Returns Health component **/
    FORCEINLINE class UEnergyComponent* GetEnergyComponent() const { return EnergyComponent; }

    /** Returns Health component **/
    FORCEINLINE class UBlinkComponent* GetBlinkComponent() const { return BlinkComponent; }

protected:
    /** Camera boom positioning the camera behind the character */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
        class USpringArmComponent* CameraBoom;

    /** Follow camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
        class UCameraComponent* TopDownCamera;
};

