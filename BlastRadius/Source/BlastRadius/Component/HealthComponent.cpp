// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthComponent.h"
#include "BlastRadius/Character/BlastRadiusCharacter.h"
#include "Gameplay/BlastRadiusPlayerState.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    CurrentHealth = 0.0f;

    OnDamage.AddDynamic(this, &UHealthComponent::TakeAnyDamage);
    //OnDeath.AddDynamic(this, &UHealthComponent::Die); //Still working on this, waiting for the Week 5 notes to be readable as an example.
}

// Called when the game starts
void UHealthComponent::BeginPlay()
{
    Super::BeginPlay();
}

//Test function for delegates, highly experimental!
void UHealthComponent::TakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
    /** FOR USE LATER WHEN REPLICATING **/
    ////Get the player state to modify current player damage
    //ABlastRadiusPlayerState* PlayerState = Cast<ABlastRadiusCharacter>(GetOwner())->GetPlayerState();
    ////Add damage to player damage
    //PlayerState->IncrementDamage(Damage);
    ////Get the current damage from the player state
    //CurrentHealth = PlayerState->GetDamage();
    CurrentHealth += Damage;

    float KnockBack;
    KnockBack = ((CurrentHealth / 10) + ((CurrentHealth * Damage) / 20)) / 500.0f;

    //Knock back impulse when projectile collides.
    Cast<ABlastRadiusCharacter>(GetOwner())->LaunchCharacter(GetOwner()->GetVelocity() * KnockBack, false, true);

    //If the characters health is below 0, make it 0.
    if (CurrentHealth < 0.f)
    {
        CurrentHealth = 0.f;
        //PlayerState->SetDamage(0.f);
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
        GEngine->AddOnScreenDebugMessage(20, 5.f, FColor::Red, "Damage %: " + CurrentHealthDisplayValue);
    }
}

//Cast Damage Taken To All Clients
void UHealthComponent::ServerTakeDamage_Implementation(FVector LaunchVelocity, float KnockBack)
{
    Cast<ABlastRadiusCharacter>(GetOwner())->LaunchCharacter(LaunchVelocity * KnockBack, false, true);
}

//Validate The Function
bool UHealthComponent::ServerTakeDamage_Validate(FVector LaunchVelocity, float KnockBack)
{
    return true;
}

void UHealthComponent::TakeDamage(float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser, FVector HitFrom)
{
    /** FOR USE LATER WHEN REPLICATING **/
    //Get the player state to modify current player damage
    ABlastRadiusPlayerState* PlayerState = Cast<ABlastRadiusCharacter>(GetOwner())->GetPlayerState();
    //Add damage to player damage
    PlayerState->IncrementDamage(Damage);
    //Get the current damage from the player state
    CurrentHealth = PlayerState->GetDamage();

    float KnockBack;
    KnockBack = ((CurrentHealth / 10) + ((CurrentHealth * Damage) / 20)) / 500.0f;

    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, "Knockback Power: " + FString::SanitizeFloat(KnockBack)); // DEBUG
                                                                                                   //Knock back impulse when projectile collides.
    FVector LaunchVelocity = HitFrom;
    LaunchVelocity.Z = 0.0f;

    ServerTakeDamage(LaunchVelocity, KnockBack);

    //If the characters health is below 0, make it 0.
    if (CurrentHealth < 0.f)
    {
        PlayerState->SetDamage(0.f);
    }
}

void UHealthComponent::Heal(float value)
{
    //Get the player state to modify current player damage
    ABlastRadiusPlayerState* PlayerState = Cast<ABlastRadiusCharacter>(GetOwner())->GetPlayerState();
    //Add Health to player damage
    PlayerState->DecrementDamage(value);
    //Get the current damage from the player state
    CurrentHealth = PlayerState->GetDamage();

    //If the characters health is below 0, make it 0.
    if (CurrentHealth < 0.f)
    {
        PlayerState->SetDamage(0.f);
    }
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UHealthComponent, CurrentHealth);
}