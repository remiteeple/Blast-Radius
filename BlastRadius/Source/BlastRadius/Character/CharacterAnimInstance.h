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
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        bool bIsBlinking;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        bool bIsAiming;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        bool bIsMoving;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        float MovementSpeed;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        float StrafingRotation;

protected:
    UCharacterAnimInstance();

    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaTime) override;

protected:
    APawn* OwningPawn;

};
