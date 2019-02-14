// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlastRadiusPlayerController.generated.h"

/**
 *
 */
UCLASS()
class BLASTRADIUS_API ABlastRadiusPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ABlastRadiusPlayerController();

    /** Setup input **/
    virtual void SetupInputComponent() override;

    /** Possess Character **/
    virtual void AcknowledgePossession(APawn* Pawn) override;

    virtual void BeginPlay() override;

    void Tick(float DeltaTime);

private:
    /** interaction methods **/
    void MoveVertical(float Scale);
    void MoveHorizontal(float Scale);

    void WalkPressed();
    void WalkReleased();

    void FirePressed();
    void FireReleased();

    void AimPressed();
    void AimReleased();

    void BlinkPressed();
    void BlinkReleased();

    void LookAtMouseCursor(float DeltaTime);

private:
    /** ABlastRadiusCharacter possession reference **/
    class ABlastRadiusCharacter* Character;

};
