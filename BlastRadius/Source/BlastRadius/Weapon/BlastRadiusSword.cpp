// Fill out your copyright notice in the Description page of Project Settings.

#include "BlastRadiusSword.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Character/BlastRadiusCharacter.h"


// Sets default values
ABlastRadiusSword::ABlastRadiusSword()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    //StaticMesh->SetupAttachment(RootComponent);
    StaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    RootComponent = StaticMesh;

    HitBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Hit Box"));
    HitBoxComponent->SetupAttachment(RootComponent);
    SetActorEnableCollision(false);
    HitBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SetActorHiddenInGame(true);
   // HitBoxComponent->OnComponentHit.AddDynamic(this, &UPrimitiveComponent::OnComponentHit);
}

// Called when the game starts or when spawned
void ABlastRadiusSword::BeginPlay()
{
	Super::BeginPlay();
    
	
}

// Called every frame
void ABlastRadiusSword::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void ABlastRadiusSword::Attach(class ABlastRadiusCharacter* Character)
{
    verify(Character != nullptr && "Weapon's attach function called with null character!");
    
        /* Set owner of this weapon.*/
        SetOwner(Character);
        SetActorHiddenInGame(true);

        // Disable weapon's physics.
        //CALL SetActorEnableCollision() to false
        SetActorEnableCollision(false);
        //CALL SetSimulatePhysics() in the Primitive and pass in false, disabling physics


        // Attach weapon to the character's mesh.
        //CALL AttachToComponent() and pass in (Character->GetSkeletalMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "WeaponSocket") <-- We are attaching this Actor to the Characters Skeletal Mesh at the WeaponSocket
        AttachToComponent(Character->GetSkeletalMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "SwordSocket");
    
}

void ABlastRadiusSword::Activate()
{
    SetActorHiddenInGame(false);
}

void ABlastRadiusSword::PutAway()
{
    
    SetActorHiddenInGame(true);
    //SetActorLocation(FVector(1000, 100000, 1000));
   

    // Detach weapon from the character's mesh.
    //DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

}

ABlastRadiusSword* ABlastRadiusSword::GetSword()
{
    return this;
}

void ABlastRadiusSword::OnComponentHit(UPrimitiveComponent * HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, FVector NormalImpulse, const FHitResult & Hit)
{
    //ABlastRadiusCharacter* Player; //= Cast<ABlastRadiusCharacter>(OtherActor);
    ABlastRadiusCharacter* temp = dynamic_cast<ABlastRadiusCharacter*>(OtherActor);
    if (temp != nullptr)
    {
        
        //Do damage to player/ enemy (OtherActor)
    }
}

