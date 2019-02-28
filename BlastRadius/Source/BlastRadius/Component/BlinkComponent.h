// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BlinkComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTRADIUS_API UBlinkComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBlinkComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    
    void Blink(AActor* Character);

    FVector GetPickableActor_LineTraceSingleByProfile(FName ProfileName, FVector & StartTrace, FVector & Direction, FVector & EndTrace);

    UPROPERTY(EditAnywhere, Category = "Audio", meta = (AllowPrivateAccess = "true"))
        class UAudioComponent* AudioComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
        class USoundBase* BlinkSound;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
};
