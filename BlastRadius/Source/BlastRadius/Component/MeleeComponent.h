// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "MeleeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTRADIUS_API UMeleeComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMeleeComponent();

    UPROPERTY(Category = "Config", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
        float Damage = 15.0f;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    void OnActorHit(AActor* Self, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
		
    void Melee();
	
};
