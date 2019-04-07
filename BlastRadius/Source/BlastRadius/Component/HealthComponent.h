// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

//OnTakeDamage delegate signature declaration
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnTakeAnyDamage, AActor*, DamagedActor, float, Damage, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

//OnDeath delegate signature declaration
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTRADIUS_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

#pragma region Members
public:
    /** Health variables **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
        float MaxHealth = 999.f; //Maximum Health of the character.
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Attributes)
        float CurrentHealth = 0.f; //Current health of the character.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
        FString CurrentHealthDisplayValue;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
        float MinHealth = 0.f;

    //Delegate function call initialization.
    FOnDeathSignature OnDeath;
    FOnTakeAnyDamage OnDamage;
#pragma endregion Members

#pragma region Methods
public:	
    //Basic function to handle damage taken. Will be modified once weapons are implemented.
    UFUNCTION(BluePrintCallable)
        void TakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

    UFUNCTION(BluePrintCallable)
        void TakeDamage(float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser, FVector HitFrom);

    //Function To Allow Damage To Be Taken On Clients
    UFUNCTION(Server, Reliable, WithValidation)
        void ServerTakeDamage(FVector LaunchVelocity, float Knockback);

    UFUNCTION(BluePrintCallable)
        FORCEINLINE float GetCurrentHealth() { return CurrentHealth; }

    FORCEINLINE void ResetKnockback() { CurrentHealth = 0; }
#pragma endregion Methods
};
