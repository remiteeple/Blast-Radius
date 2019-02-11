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
	class UCameraComponent* TopDownCamera;
public:
	ABlastRadiusCharacter();

	void PostInitializeComponents();

	void BeginPlay();

	void Tick(float DeltaTime);

	/** Called when actor hit **/
	void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Defines character max walk speed **/
	UPROPERTY(EditDefaultsOnly)
		float MaxWalkSpeed;

	/** Defines character max run speed **/
	UPROPERTY(EditDefaultsOnly)
		float MaxRunSpeed;

	/** Called for directional movement */
	void Move(FVector Direction, float Scale);

	/** Called for directional rotation of character  **/
	void LookAt(FVector Direction);

	/** Called for aiming **/
	//void Aim();

	/** Call for shooting **/
	//void Shoot();

public:
	/** Determines states **/
	// Default false
	bool bIsMoving = false;
	bool bIsWalking = false;
	bool bIsAiming = false;
	bool bIsFiring = false;
	bool bIsBlinking = false;

public:
	/** Component declarations **/
	//class UHealthComponent* HealthComponent;   // waiting for component implementation
	//class UEnergyComponent* EnergyComponent;   // waitinf for component implementation
	class USkeletalMeshComponent* SkeletalMesh;
	class UBlinkComponent* BlinkComponent;
	//class UCharacterAnimInstance* AnimationInstance;

	/** Weapon the character uses **/
	//UPROPERTY()
	//    class AWeaponBase* Weapon; // Weapon

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return TopDownCamera; }

	/** Returns SkeletalMesh subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetSkeletalMesh() const { return SkeletalMesh; }

};

