// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BLASTRADIUS_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
    /** Animation states **/
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        bool bIsBlinking;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        bool bIsMeleeAttacking;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        bool bIsFiring;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        bool bIsMoving;

    /** Animation variables **/
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        float MovementSpeed;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        float StrafingRotation;

protected:
    UCharacterAnimInstance();

protected:
    APawn* OwningPawn;

};
