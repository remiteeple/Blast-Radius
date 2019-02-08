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

    void SetupRay(FVector & StartTrace, FVector & Direction, FVector & EndTrace);
		
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
        float BlinkValue = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
        float BlinkDistance = 250.0f;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
};
