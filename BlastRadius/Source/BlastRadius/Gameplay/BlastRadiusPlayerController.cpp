// Fill out your copyright notice in the Description page of Project Settings.

#include "BlastRadiusPlayerController.h"
#include "Character/BlastRadiusCharacter.h"

ABlastRadiusPlayerController::ABlastRadiusPlayerController()
{
    //TODO remove before final
    bShowMouseCursor = true;
}

void ABlastRadiusPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Set up gameplay key bindings
    InputComponent->BindAxis("MoveVertical", this, &ABlastRadiusPlayerController::MoveVertical);
    InputComponent->BindAxis("MoveHorizontal", this, &ABlastRadiusPlayerController::MoveHorizontal);

    InputComponent->BindAction("Fire", IE_Pressed, this, &ABlastRadiusPlayerController::FirePressed);
    InputComponent->BindAction("Fire", IE_Released, this, &ABlastRadiusPlayerController::FireReleased);

    InputComponent->BindAction("Aim", IE_Pressed, this, &ABlastRadiusPlayerController::AimPressed);
    InputComponent->BindAction("Aim", IE_Released, this, &ABlastRadiusPlayerController::AimReleased);

    InputComponent->BindAction("Blink", IE_Pressed, this, &ABlastRadiusPlayerController::BlinkPressed);
    InputComponent->BindAction("Blink", IE_Released, this, &ABlastRadiusPlayerController::BlinkReleased);
}

void ABlastRadiusPlayerController::AcknowledgePossession(APawn* PossesedPawn)
{
    Super::AcknowledgePossession(PossesedPawn);

    Character = Cast<ABlastRadiusCharacter>(PossesedPawn);
    check(Character != nullptr && "Player Controller did not receive a valid ABlastRadiusCharacter!");
}

void ABlastRadiusPlayerController::BeginPlay()
{
    Super::BeginPlay();
}

void ABlastRadiusPlayerController::MoveVertical(float Scale)
{
    if (!Character)
        return;

    /* Move the character along the X-axis */
    Character->Move(FVector(1.0f, 0.0f, 0.0f), Scale);
}

void ABlastRadiusPlayerController::MoveHorizontal(float Scale)
{
    if (!Character)
        return;

    /* Move the character along the Y-axis */
    Character->Move(FVector(0.0f, 1.0f, 0.0f), Scale);
}

void ABlastRadiusPlayerController::FirePressed()
{
    if (!Character)
        return;
}

void ABlastRadiusPlayerController::FireReleased()
{
    if (!Character)
        return;
}

void ABlastRadiusPlayerController::AimPressed()
{
    if (!Character)
        return;
}

void ABlastRadiusPlayerController::AimReleased()
{
    if (!Character)
        return;
}

void ABlastRadiusPlayerController::BlinkPressed()
{
    if (!Character)
        return;
}

void ABlastRadiusPlayerController::BlinkReleased()
{
    if (!Character)
        return;
}
