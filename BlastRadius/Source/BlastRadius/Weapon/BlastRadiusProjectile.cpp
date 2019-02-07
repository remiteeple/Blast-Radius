// Fill out your copyright notice in the Description page of Project Settings.

#include "BlastRadiusProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

// Sets default values
ABlastRadiusProjectile::ABlastRadiusProjectile()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComp->InitSphereRadius(5.0f);
    CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
    CollisionComp->OnComponentHit.AddDynamic(this, &ABlastRadiusProjectile::OnHit);
    CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
    CollisionComp->CanCharacterStepUpOn = ECB_No;

    //root
    RootComponent = CollisionComp;

    //Mesh "Laser"
    ProjectileMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    ProjectileMeshComp->SetupAttachment(RootComponent);

    // Use a ProjectileMovementComponent to govern this projectile's movement
    ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
    ProjectileMovementComp->UpdatedComponent = CollisionComp;
    ProjectileMovementComp->InitialSpeed = 3000.f;
    ProjectileMovementComp->MaxSpeed = 3000.f;
    ProjectileMovementComp->bRotationFollowsVelocity = true;
    ProjectileMovementComp->bShouldBounce = true;
    //ProjectileMovementComp->SetupAttachment(RootComponent);

    //Laser's lifespan.
    m_LifeSpan = 5.0f;

    //test.
}

// Called when the game starts or when spawned
void ABlastRadiusProjectile::BeginPlay()
{
    Super::BeginPlay();

}

// Called every frame
void ABlastRadiusProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}


void ABlastRadiusProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
    {
        //if (OtherActor == AWallActor)
        //{
        //reflect(calculate angle)
        //return;
        //}
    }

    //Instead of 100.0f use, percentage amount of player knock-back.
    OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

    //if(OtherActor == ACharacter) 
    //{
    //    OtherActor->TakeDamage(m_pLaserDamage)
    //}

    //DestroySelf();

}

