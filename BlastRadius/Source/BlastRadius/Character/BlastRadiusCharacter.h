// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlastRadiusCharacter.generated.h"

UCLASS(config=Game)
class ABlastRadiusCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	ABlastRadiusCharacter();

    void PostInitializeComponents();

    void BeginPlay();

    void Tick(float DeltaTime);

    /** Defines character max walk speed **/
    UPROPERTY(EditDefaultsOnly)
        float MaxWalkSpeed;

    /** Defines character max run speed **/
    UPROPERTY(EditDefaultsOnly)
        float MaxRunSpeed;

	/** Called for directional movement */
	void Move(FVector Direction, float Scale);

private:
    /** Determines states **/
    bool bIsWalking;
    bool bIsAiming;
    bool bIsFiring;
    bool bIsBlinking;

    /** Component declarations **/
    //class UHealthComponent* Health;   // waiting for component implementation
    //class UEnergyComponent* Energy;   // waitinf for component implementation
    //class UBlinkComponent* Blink;     // waiting for component implementation
    class USkeletalMeshComponent* SkeletalMesh;
    //class UCharacterAnimInstance* AnimationInstance;

    /** Weapon the character uses **/
    //UPROPERTY()
    //    class AWeaponBase* Weapon; // Weapon

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

    /** Returns SkeletalMesh subobject **/
    FORCEINLINE class USkeletalMeshComponent* GetSkeletalMesh() const { return SkeletalMesh; }

};

