// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlastRadiusCharacter.generated.h"

UCLASS(config = Game)
class ABlastRadiusCharacter : public ACharacter
{
    GENERATED_BODY()

protected:
    ABlastRadiusCharacter();

    virtual void PostInitializeComponents() override;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

#pragma region Members
public:
    /*  Energy Spendature Values  */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Energy)
        float BlinkCost = 35.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Energy)
        float ShootCost = 15.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Energy)
        float MeleeCost = 25.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Energy)
        float GrenadeCost = 25.0f;
protected:
    /* Replicated Orientation */
    UPROPERTY(Replicated)
        FVector Orientation;

    /*  Character Movement Values  */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
        float MaxWalkSpeed;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
        float MaxRunSpeed;

    /*  Spawn location variable  */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
        FVector SpawnPoint;
    /*  Spawn timer  */
    FTimerHandle TimerHandle_SpawnTimer;
    /*  Spawn Delay */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
        float SpawnDelay;

    /*  Animations  */
    UPROPERTY(EditDefaultsOnly)
        UAnimMontage* HipFireAnimation;
    UPROPERTY(EditDefaultsOnly)
        UAnimMontage* MeleeAttackAnimation;
    UPROPERTY(EditDefaultsOnly)
        FTimerHandle TimerHandle_MeleeTimer;

    /*  Audio  */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
        class USoundBase* DeathSound;

    /* Templates */
    UPROPERTY(EditDefaultsOnly, Category = Projectile)
        TSubclassOf<class ABlastRadiusProjectile> ProjectileClass;
    UPROPERTY(EditDefaultsOnly, Category = Projectile)
        TSubclassOf<class ABlastRadiusGrenade> GrenadeClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
        TSubclassOf<class ABlastRadiusSword> SwordClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
        TSubclassOf<class ABlastRadiusWeapon> WeaponClass;

    /* Weapons */
    UPROPERTY(Replicated)
        ABlastRadiusSword* Sword;
    UPROPERTY(Replicated)
        ABlastRadiusWeapon* Weapon;

public:
    /* Team Variables */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MPCharacter|Gameplay", Replicated)// ReplicatedUsing = OnRep_ChangeColor)//meta = (ClampMin = "1", ClampMax = "2"), Replicated
        int playerTeam;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MPCharacter|Debug", Replicated) //meta = (EditCondition = "AreTeamsEnabled", ClampMin = "0")
        int TeamOneCount;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MPCharacter|Debug", Replicated) // , meta = (EditCondition = "AreTeamsEnabled", ClampMin = "0")
        int TeamTwoCount;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MPCharacter|Debug", Replicated)
        int NetIndex;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MPCharacter|Mesh", Replicated)
        class UMaterialInterface* DefaultTPMaterials;

    /* Team Update */
    FTimerHandle UpdateHandle;
    UPROPERTY(Replicated)
        FTimerHandle PostBeginPlayDelay;
#pragma endregion Members

#pragma region Methods
public:
    /* Raycast */
    bool GetPickableActor_LineTraceTestByObjectType(EObjectTypeQuery ObjectType);
    void SetupRay(FVector &StartTrace, FVector &Direction, FVector &EndTrace);

    /* Team Functions */
    void AssignTeams();
    void AssignNetIndex();
    UFUNCTION(NetMulticast, Reliable)
        void Multicast_AssignTeamsColor();
    UFUNCTION(BlueprintCallable)
        virtual void UpdateAndCheckPlayer();
    UFUNCTION(BlueprintCallable)
        void PostBeginPlay();

    /*  Called for directional movement */
    void Move(FVector Direction, float Scale);

    /*  Called for directional rotation of character   */
    void LookAt(FVector Direction);
    /*  Server Call for directional rotation of character   */
    UFUNCTION(Server, Reliable, WithValidation)
        void ServerLookAt(FVector Direction);

    /*  Called when blink is activated  */
    void Blink();
    /*  Server Call when blink is activated  */
    UFUNCTION(Server, Reliable, WithValidation)
        void ServerBlink();

    /* Call for grenade */
    void LobGrenade();
    UFUNCTION(Server, Reliable, WithValidation)
        void ServerLobGrenade();

    /*  Call for shooting  */
    void Fire();
    /* Server Call for Shooting */
    UFUNCTION(Server, Reliable, WithValidation)
        void ServerFire();

    /*  Called for melee attack  */
    void Melee();
    /*  Called to hide sword after melee  */
    void PutAwaySword();

    /*  Called on character death  */
    void OnDeath();
    /*  Called when player health passes lower limit  */
    UFUNCTION(NetMulticast, Reliable, WithValidation)
        void NetMultiCastOnDeath();

    /*  Getter for Player State  */
    UFUNCTION(BlueprintCallable)
        class ABlastRadiusPlayerState* GetPlayerState();
    /*  Getter for Game State  */
    UFUNCTION(BlueprintCallable)
        class ABlastRadiusGameStateBase* GetGameState();

    /* Teleports player upon stock loss to spawn point  */
    void Respawn();

    /*  Handle character overlap  */
    UFUNCTION()
        void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
#pragma region Methods

#pragma region States
public:
    /*  State Definitions  */
    // Default false
    bool bIsWalking = false;
    bool bIsAiming = false;
    bool bIsMeleeAttacking = false;
    bool bIsFiring = false;
    bool bIsBlinking = false;
    bool bIsDead = false;
#pragma endregion States

#pragma region Getters
public:
    /*  Returns CameraBoom subobject  */
    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

    /*  Returns FollowCamera subobject  */
    FORCEINLINE class UCameraComponent* GetFollowCamera() const { return TopDownCamera; }

    /*  Returns SkeletalMesh component  */
    FORCEINLINE class USkeletalMeshComponent* GetSkeletalMesh() const { return SkeletalMesh; }

    /*  Returns Health component  */
    FORCEINLINE class UHealthComponent* GetHealthComponent() const { return HealthComponent; }

    /*  Returns Health component  */
    FORCEINLINE class UEnergyComponent* GetEnergyComponent() const { return EnergyComponent; }

    /*  Returns Health component  */
    FORCEINLINE class UBlinkComponent* GetBlinkComponent() const { return BlinkComponent; }
#pragma endregion Getters

#pragma region Components
protected:
    /*  Camera boom positioning the camera behind the character */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
        class USpringArmComponent* CameraBoom;

    /*  Follow camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
        class UCameraComponent* TopDownCamera;

    /*  Helmet Mesh  */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Helmet, meta = (AllowPrivateAccess = "true"))
        class UStaticMeshComponent* HelmetMesh;

    /*  Audio Component  */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Audio, meta = (AllowPrivateAccess = "true"))
        class UAudioComponent* AudioComponent;

    /* Arrow Component */
    class UArrowComponent* ArrowComponent;

    /* Skeletal Mesh Component */
    class USkeletalMeshComponent* SkeletalMesh;

    /* Capsule Component (Collider) */
    class UCapsuleComponent* CapsuleCollider;

    /* Character Animation Instance */
    class UCharacterAnimInstance* AnimationInstance;

    /* Blink Component */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Blink, meta = (AllowPrivateAccess = "true"))
        class UBlinkComponent* BlinkComponent;

    /* Health Component */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
        class UHealthComponent* HealthComponent;

    /* Energy Component */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Energy, meta = (AllowPrivateAccess = "true"))
        class UEnergyComponent* EnergyComponent;
#pragma endregion Components
};

