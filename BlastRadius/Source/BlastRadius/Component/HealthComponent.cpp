// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthComponent.h"


// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
    CurrentHealth = 0.0f;
    //
    OnDamage.AddDynamic(this, &UHealthComponent::TakeAnyDamage);
    //OnDeath.AddDynamic(this, &UHealthComponent::Die); //Still working on this, waiting for the Week 5 notes to be readable as an example.

}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UHealthComponent::TakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
    CurrentHealth -= Damage;

    //Add the damage to the "health" %
    CurrentHealth += Damage;



    //If the characters health is below 0, make it 0.
    if (CurrentHealth < 0.f)
    {
        CurrentHealth = 0.f;
    }
}

// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    CurrentHealthDisplayValue = FString::SanitizeFloat(CurrentHealth);

    //For Debugging, remove on HUD implementation.
    GEngine->AddOnScreenDebugMessage(5, 5.f, FColor::Cyan, FString::Printf(TEXT("Current Health: %x / %y"), CurrentHealth, MaxHealth));
	// ...
}

