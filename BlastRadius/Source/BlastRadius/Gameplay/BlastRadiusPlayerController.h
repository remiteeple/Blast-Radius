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

private:
    /** Movement & interaction methods **/
    void MoveVertical(float Scale);
    void MoveHorizontal(float Scale);

    void FirePressed();
    void FireReleased();
    void AimPressed();
    void AimReleased();

    void BlinkPressed();
    void BlinkReleased();

private:
    /** Character possession reference **/
    class ABlastRadiusCharacter* Character;

};
