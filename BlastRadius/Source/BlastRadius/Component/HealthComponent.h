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

    //Maximum Health of the character.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
        float MaxHealth = 100.f;

    //Current health of the character.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
        float CurrentHealth = 100.f;

    //Delegate function call initialization.
    FOnDeathSignature OnDeath;
    FOnTakeAnyDamage OnDamage;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

    //Basic function to handle damage taken. Will be modified once weapons are implemented.
    UFUNCTION(BluePrintCallable)
        void TakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);



public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
	
};