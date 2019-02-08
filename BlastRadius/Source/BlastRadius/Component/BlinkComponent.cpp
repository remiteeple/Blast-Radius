// Fill out your copyright notice in the Description page of Project Settings.

#include "BlinkComponent.h"
#include "BlastRadius/Character/BlastRadiusCharacter.h"
#include "GameFramework/Controller.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"


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

        FVector StartTrace = Character->GetActorLocation();
        FVector Direction = Character->GetActorRotation().Vector();
        FVector EndTrace = StartTrace + Direction * BlinkDistance;;

        SetupRay(StartTrace, Direction, EndTrace);

        Character->SetActorLocation(FMath::Lerp(StartTrace, EndTrace, 1));
    }
}

void UBlinkComponent::SetupRay(FVector &StartTrace, FVector &Direction, FVector &EndTrace)
{
    //StartTrace = CamLoc; // trace start is the camera location
    //Direction = CamRot.Vector();
    EndTrace = StartTrace + Direction * 300; // and trace end is the camera location + an offset in the direction you are looking, the 200 is the distance at wich it checks

}