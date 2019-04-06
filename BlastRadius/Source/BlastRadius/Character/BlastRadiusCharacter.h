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
protected:
    /* Replicated Orientation */
    UPROPERTY(Replicated)
        FVector Orientation;

    /*  Character Movement Values  */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
        float MaxWalkSpeed;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
        float MaxRunSpeed;

    /*  Energy Spendature Values  */
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Energy_Cost)
        float BlinkCost = 35.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Energy_Cost)
        float ShootCost = 15.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Energy_Cost)
        float MeleeCost = 25.0f;

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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
        TSubclassOf<class ABlastRadiusSword> SwordClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
        TSubclassOf<class ABlastRadiusWeapon> WeaponClass;

    UPROPERTY(Replicated)
        ABlastRadiusSword* Sword;
    UPROPERTY(Replicated)
        ABlastRadiusWeapon* Weapon;

public:
    /* Team Variables */
    //TODO Week 7:The player's current team.
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MPCharacter|Gameplay", Replicated)// ReplicatedUsing = OnRep_ChangeColor)//meta = (ClampMin = "1", ClampMax = "2"), Replicated
        int playerTeam;
    //TODO Week 7:Size of team one (grabbed from game state)
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MPCharacter|Debug", Replicated) //meta = (EditCondition = "AreTeamsEnabled", ClampMin = "0")
        int TeamOneCount;
    //TODO Week 7: Size of team two (grabbed from game state)
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MPCharacter|Debug", Replicated) // , meta = (EditCondition = "AreTeamsEnabled", ClampMin = "0")
        int TeamTwoCount;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MPCharacter|Debug", Replicated)
        int NetIndex;
    //TODO Week 7: Players Material Color
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MPCharacter|Mesh", Replicated)
        class UMaterialInterface* DefaultTPMaterials;
    //TODO Week 7: Update Player Timer
    //Calls UpdateAndCheckPlayer
    FTimerHandle UpdateHandle;

    //TODO Week 7: PostBeginPlay Timer`
    UPROPERTY(Replicated)
        FTimerHandle PostBeginPlayDelay;
#pragma endregion Members

#pragma region Methods
public:
    /* Team Functions */
    void AssignTeams();
    //TODO Week 7: Assigns a Network Index to the player that logs in
    void AssignNetIndex();
    //TODO Week 7: Multicasts to all clients to assign the team color to Simulated_Proxy's and Autonomous_Proxy's when player logs in(CALLED FROM SERVER)
    UFUNCTION(NetMulticast, Reliable)
        void Multicast_AssignTeamsColor();
    UFUNCTION(BlueprintCallable, Category = FPSWizard)
        virtual void UpdateAndCheckPlayer();
    UFUNCTION(BlueprintCallable, Category = FPSWizard)
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

    /*  Teleports player upon stock loss to spawn point  */
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

