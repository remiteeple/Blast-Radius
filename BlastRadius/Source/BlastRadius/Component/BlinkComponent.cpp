// Fill out your copyright notice in the Description page of Project Settings.

#include "BlinkComponent.h"
#include "BlastRadius/Character/BlastRadiusCharacter.h"
#include "GameFramework/Controller.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"


// Sets default values for this component's properties
UBlinkComponent::UBlinkComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

    //Setup the Audio Component
    AudioComponent = CreateDefaultSubobject<UAudioComponent>("BlinkSound");
    AudioComponent->bAutoActivate = false;
    AudioComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
}


// Called when the game starts
void UBlinkComponent::BeginPlay()
{
	Super::BeginPlay();

    //Particles appear when blink initiated.

}


// Called every frame
void UBlinkComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UBlinkComponent::Blink(AActor* Character)
{
        ABlastRadiusCharacter* Player = Cast<ABlastRadiusCharacter>(Character);
        FVector StartTrace = Player->GetActorLocation();
        FVector Direction = Player->GetActorRotation().Vector();
        FVector EndTrace = StartTrace + Direction * 300.f;

        FVector BlinkPosition = EndTrace;//GetPickableActor_LineTraceSingleByProfile("Visible", StartTrace, Direction, EndTrace);

        //Player = Cast<ABlastRadiusCharacter>(GetOwner());
        FVector InitialPosition = Player->GetActorLocation();

        Character->SetActorLocation(FMath::Lerp(InitialPosition, BlinkPosition, 1));

        //Play the audio for blinking
        AudioComponent->SetSound(BlinkSound);
        AudioComponent->Play();
}

FVector UBlinkComponent::GetPickableActor_LineTraceSingleByProfile(FName ProfileName, FVector & StartTrace, FVector & Direction, FVector & EndTrace)
{
   FCollisionQueryParams TraceParams;
   TraceParams.AddIgnoredActor(GetOwner());
   TraceParams.bTraceComplex = true;
   TraceParams.bReturnPhysicalMaterial = true;
   
   FHitResult Hit(ForceInit);
   UWorld* World = GetWorld();
   World->LineTraceSingleByProfile(Hit, StartTrace, EndTrace, ProfileName, TraceParams); // simple trace function "Pawn"
   DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green, true, 1, 0, 1.f);
   
   return Hit.GetActor()->GetActorLocation();   
}