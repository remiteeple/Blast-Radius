// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthComponent.h"
#include "BlastRadius/Character/BlastRadiusCharacter.h"


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
//Test function for delegates, highly experimental!
void UHealthComponent::TakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{

    //Add the damage to the "health" %
    CurrentHealth += Damage;

    float KnockBack;
    KnockBack = ((CurrentHealth / 10) + ((CurrentHealth * Damage) / 20)) / 500.0f;

    //Knock back impulse when projectile collides.
    Cast<ABlastRadiusCharacter>(GetOwner())->LaunchCharacter(GetOwner()->GetVelocity() * KnockBack, false, true);

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

    if (GetOwner())
    {
        //Shows current character health. For Debugging, remove on HUD implementation.
        GEngine->AddOnScreenDebugMessage(20, 5.f, FColor::Red, *CurrentHealthDisplayValue);
    }
    
	// ...
}

void UHealthComponent::TakeDamage(float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser, FVector HitFrom)
{
    //Add the damage to the "health" %
    CurrentHealth += Damage;

    float KnockBack;
    KnockBack = ((CurrentHealth / 10) + ((CurrentHealth * Damage) / 20)) / 500.0f;

    //GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, FString::SanitizeFloat(KnockBack)); // DEBUG

                                                                                                   //Knock back impulse when projectile collides.
    FVector LaunchVelocity = HitFrom;
    LaunchVelocity.Z = 0.0f;
    Cast<ABlastRadiusCharacter>(GetOwner())->LaunchCharacter(LaunchVelocity * KnockBack, false, true);

    //If the characters health is below 0, make it 0.
    if (CurrentHealth < 0.f)
    {
        CurrentHealth = 0.f;
    }
}