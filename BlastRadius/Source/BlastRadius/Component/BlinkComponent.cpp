// Fill out your copyright notice in the Description page of Project Settings.

#include "BlinkComponent.h"
#include "BlastRadius/Character/BlastRadiusCharacter.h"
#include "GameFramework/Controller.h"


// Sets default values for this component's properties
UBlinkComponent::UBlinkComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBlinkComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBlinkComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UBlinkComponent::Blink(AActor* Character)
{

    AController* CharacterController = Cast<ABlastRadiusCharacter>(Character)->Controller;
    if ((CharacterController != NULL))
    {
        const FRotator Rotation = CharacterController->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get forward vector
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

        Cast<ABlastRadiusCharacter>(Character)->AddMovementInput(Direction, BlinkValue);
    }
}

