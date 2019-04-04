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

    /**Team Variables**/
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

    //TODO Week 7: PostBeginPlay Timer
    UPROPERTY(Replicated)
        FTimerHandle PostBeginPlayDelay;
    /**Team Functions**/
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
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerBlink();

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

    /** Called on character death **/
    void OnDeath();

    /** Getter for Player State **/
    UFUNCTION(BlueprintCallable)
    class ABlastRadiusPlayerState* GetPlayerState();
    /** Getter for Game State **/
    UFUNCTION(BlueprintCallable)
    class ABlastRadiusGameStateBase* GetGameState();

    /** Teleports player upon stock loss to spawm point **/
    void Respawn();

    /** Handle character overlap **/
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

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

    class UCapsuleComponent* CapsuleCollider;

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

    /** Helmet Mesh **/
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Helmet, meta = (AllowPrivateAccess = "true"))
        class UStaticMeshComponent* HelmetMesh;

    /** Energy Spendature Values **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Energy_Cost)
        float BlinkCost = 35.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Energy_Cost)
        float ShootCost = 15.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Energy_Cost)
        float MeleeCost = 25.0f;
};

