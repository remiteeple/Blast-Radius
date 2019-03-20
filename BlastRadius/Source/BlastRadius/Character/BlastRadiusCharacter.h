// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlastRadiusCharacter.generated.h"

UCLASS(config = Game)
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
    /**Replicated Orientation**/
    UPROPERTY(Replicated)
        FVector Orientation;

    /** Spawn location variable **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
        FVector SpawnPoint;
    /** Spawn timer **/
    FTimerHandle TimerHandle_SpawnTimer;
    /** Spawn Delay**/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
        float SpawnDelay;

    /** Animations **/
    UPROPERTY(EditDefaultsOnly)
        UAnimMontage* HipFireAnimation;
    UPROPERTY(EditDefaultsOnly)
        UAnimMontage* MeleeAttackAnimation;
    UPROPERTY(EditDefaultsOnly)
        FTimerHandle TimerHandle_MeleeTimer;

    //Projectile template to fire.
    UPROPERTY(EditDefaultsOnly, Category = Projectile)
        TSubclassOf<class ABlastRadiusProjectile> ProjectileClass;
    //Sword Template to use
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
        TSubclassOf<class ABlastRadiusSword> SwordClass;
    UPROPERTY(Replicated)
    ABlastRadiusSword* Sword;
    /* Weapon */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
        TSubclassOf<class ABlastRadiusWeapon> WeaponClass;
    UPROPERTY(Replicated)
    ABlastRadiusWeapon* Weapon;

    /* Blink particles*/
    UPROPERTY(EditDefaultsOnly, Category = "Effects")
        class UParticleSystemComponent* PSC;
    UPROPERTY(EditDefaultsOnly, Category = "Effects")
        UParticleSystem* ProjectileFX;

    /** Called for directional movement */
    void Move(FVector Direction, float Scale);

    /** Called for directional rotation of character  **/
    void LookAt(FVector Direction);
    /** Server Call for directional rotation of character  **/
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerLookAt(FVector Direction);

    /** Called when blink is activated **/
    void Blink();
    /** Server Call when blink is activated **/
   // void ServerBlink();

    /** Call for shooting **/
    void Fire();

    /*Server Call for Shooting*/
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerFire();
    /** Called for melee attack **/
    void Melee();
    /** Sever Call for melee attack **/
   // UFUNCTION(Server, Reliable, WithValidation)
  //void ServerMelee();
    /** Called to hide sword after melee **/
    void PutAwaySword();

    /** Called when player health passes lower limit **/
    UFUNCTION(NetMulticast, Reliable, WithValidation)
        void NetMultiCastOnDeath();

    void OnDeath();

    /** Getter for Player State **/
    UFUNCTION(BlueprintCallable)
    class ABlastRadiusPlayerState* GetPlayerState();
    /** Getter for Game State **/
    UFUNCTION(BlueprintCallable)
    class ABlastRadiusGameStateBase* GetGameState();


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
    class USkeletalMeshComponent* SkeletalMesh;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blink", meta = (AllowPrivateAccess = "true"))
        class UBlinkComponent* BlinkComponent;
    //class UMeleeComponent* MeleeComponent;
    class UCharacterAnimInstance* AnimationInstance;

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
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
        class UCameraComponent* TopDownCamera;

    /** Energy Spendature Values **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Energy_Cost)
        float BlinkCost = 35.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Energy_Cost)
        float ShootCost = 15.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Energy_Cost)
        float MeleeCost = 25.0f;
};

