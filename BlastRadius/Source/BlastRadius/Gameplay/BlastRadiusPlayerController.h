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

    void Tick(float DeltaTime) override;

#pragma region Methods
private:
    /** Interaction methods **/
    void AimForward(float Scale);
    void AimRight(float Scale);

    void LookRight(float Scale);

    void MoveVertical(float Scale);
    void MoveHorizontal(float Scale);

    void WalkPressed();
    void WalkReleased();

    void GrenadePressed();
    void GrenadeReleased();

    void FirePressed();
    void FireReleased();

    void BlinkPressed();
    void BlinkReleased();
    
    void MeleePressed();
    void MeleeReleased();

    void LookAtMouseCursor(float DeltaTime);
#pragma endregion Methods

#pragma region Members
private:
    /* ABlastRadiusCharacter possession reference */
    class ABlastRadiusCharacter* Character;

    /* Control members */
    FVector ThumbStickDir;

    float LookRightValue;
    float LookForwardValue;
    FRotator LookDirection;

    float CameraSensitivity = 25.0f;
#pragma endregion Members
};
