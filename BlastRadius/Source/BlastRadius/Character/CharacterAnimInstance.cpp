// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterAnimInstance.h"

UCharacterAnimInstance::UCharacterAnimInstance() :
    bIsBlinking(false),
    bIsAiming(false),
    bIsMoving(false),
    MovementSpeed(0.0f),
    StrafingRotation(0.0f),
    OwningPawn(nullptr)
{
}

void UCharacterAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    OwningPawn = TryGetPawnOwner();
}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);
}
