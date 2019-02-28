// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterAnimInstance.h"

UCharacterAnimInstance::UCharacterAnimInstance() :
    bIsBlinking(false),
    bIsMeleeAttacking(false),
    bIsMoving(false),
    MovementSpeed(0.0f),
    StrafingRotation(0.0f),
    OwningPawn(nullptr)
{

}