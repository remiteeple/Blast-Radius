// Fill out your copyright notice in the Description page of Project Settings.

#include "BlastRadiusProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Character/BlastRadiusCharacter.h"
#include "Runtime/Engine/Classes/GameFramework/DamageType.h"
#include "Component/HealthComponent.h"

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
    CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

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
   
    //Template for HealthComponent->TakeDamage() parameter.
    m_DamageType = UDamageType::StaticClass();

    //Laser's lifespan.
    m_LifeSpan = 3.0f;

    //Damage
    m_LaserDamage = 50.0f;
    
    //max amount of bounces until object is destroyed.
    m_MaxBounceAmount = 5.0f;

    //Knock back Amount for collision with projectile. This might be Health percentage * 10 later.
    m_KnockbackFactor = 100.0f;


    /*
    In Blueprint editor:
    For a multiplayer game, We'll need to uncheck "Initial Velocity in Local Space" 
    in the "MovementComp" Component in order for this projectile to replicate correctly over a server.
    */
}

// Called when the game starts or when spawned
void ABlastRadiusProjectile::BeginPlay()
{
    Super::BeginPlay();

    GetWorld()->GetTimerManager().SetTimer(SpawnTimer,
        this,
        &ABlastRadiusProjectile::DestroySelf,
        m_LifeSpan, true);
}

// Called every frame
void ABlastRadiusProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}


void ABlastRadiusProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics() && OtherActor != GetOwner())
    {
        //handles collision handle for characters
        if (OtherActor->ActorHasTag("Player"))
        {
            //Calling TakeDamage on the otherActor's HealthComponent. 
            FDamageEvent DamageEvent;
            Cast<ABlastRadiusCharacter>(OtherActor)->TakeDamage(m_LaserDamage, DamageEvent, OtherActor->GetInstigatorController(), this->GetOwner());


            //Knock back impulse when projectile collides.
            OtherComp->AddImpulseAtLocation(GetVelocity() * m_KnockbackFactor, GetActorLocation());
            DestroySelf();
        }

        //Decrement bounce when hitting walls until bounce limit is hit.
        if (m_MaxBounceAmount != 0)
        {
            if (OtherActor->ActorHasTag("Wall"))
            {
                m_MaxBounceAmount--;
            }
        }
        else
        {
            DestroySelf();
        }
    }
}


void ABlastRadiusProjectile::FireInDirection(const FVector& ShootDirection)
{
    ProjectileMovementComp->Velocity = ShootDirection * ProjectileMovementComp->InitialSpeed;
}


void ABlastRadiusProjectile::DestroySelf()
{
    this->Destroy();
}