// Fill out your copyright notice in the Description page of Project Settings.

#include "BlastRadiusProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Character/BlastRadiusCharacter.h"
#include "Runtime/Engine/Classes/GameFramework/DamageType.h"
#include "Component/HealthComponent.h"
#include "Runtime/Engine/Classes/GameFramework/DamageType.h"

// Sets default values
ABlastRadiusProjectile::ABlastRadiusProjectile()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComp->InitSphereRadius(50.0f);
    CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
    CollisionComp->OnComponentHit.AddDynamic(this, &ABlastRadiusProjectile::OnHit);
    CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
    CollisionComp->CanCharacterStepUpOn = ECB_No;
    CollisionComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);

    //CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    ////SET the OverlapComponent's Collision to ignore all channels
    //CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
    ////SET the OverlapComponents to respond to the Pawn channel and only Overlap events
    //CollisionComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
    ////SET the OverlapComponent to be visible in Game
    //CollisionComp->SetHiddenInGame(false);

    //root
    RootComponent = CollisionComp;

    //Mesh "Laser"
    ProjectileMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    ProjectileMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    ProjectileMeshComp->SetupAttachment(RootComponent);

    // Use a ProjectileMovementComponent to govern this projectile's movement
    ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
    ProjectileMovementComp->UpdatedComponent = CollisionComp;
    ProjectileMovementComp->InitialSpeed = 3000.f;
    ProjectileMovementComp->MaxSpeed = 3000.f;
    ProjectileMovementComp->bRotationFollowsVelocity = true;
    ProjectileMovementComp->bShouldBounce = true;
    ProjectileMovementComp->ProjectileGravityScale = 0.0f;
    ProjectileMovementComp->Bounciness = 1.0f;
    ProjectileMovementComp->Friction = 0.0f;
    ProjectileMovementComp->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::Z);


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
    //AActor* temp = GetOwner();
    //AActor* tempChar = GetOwner()->GetOwner();
    //bool woo = false;
    //if (temp)
    //{
    //    OtherActor->SetOwner(OtherActor);
    //}
    //if (tempChar)
    //{
    //    OtherActor->SetOwner(OtherActor);
    //}

    if ((OtherActor != NULL) && 
        (OtherActor != this) && 
        (OtherComp != NULL) && 
        OtherActor != GetOwner())
    {
        //handles collision handle for characters
        if (Cast<ABlastRadiusCharacter>(OtherActor) != nullptr)
        {
            //Calling TakeDamage on the otherActor's HealthComponent. 
            const UDamageType* Laser_DamageType = Cast<UDamageType>(UDamageType::StaticClass());
            Cast<ABlastRadiusCharacter>(OtherActor)->GetHealthComponent()->TakeDamage(m_LaserDamage, Laser_DamageType, OtherActor->GetInstigatorController(), GetOwner(), GetVelocity());

            //OLD KNOCKBACK CODE: Moved to health component!
            /*
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "OtherActor CurrentHealth - " + FString::SanitizeFloat(OtherCharacter->GetHealthComponent()->GetCurrentHealth())); // DEBUG

            float CurrentHealth = Cast<ABlastRadiusCharacter>(OtherCharacter)->GetHealthComponent()->GetCurrentHealth();

            float KnockBack;
            KnockBack = ((CurrentHealth / 10) + ((CurrentHealth * m_LaserDamage) / 20)) / 500.0f;

            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, FString::SanitizeFloat(KnockBack)); // DEBUG

            //Knock back impulse when projectile collides.
            FVector LaunchVelocity = GetVelocity();
            LaunchVelocity.Z = 0.0f;
            Cast<ABlastRadiusCharacter>(OtherCharacter)->LaunchCharacter(LaunchVelocity * KnockBack, false, true);
            */

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