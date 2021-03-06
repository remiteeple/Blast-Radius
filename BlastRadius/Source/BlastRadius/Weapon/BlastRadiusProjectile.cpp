// Fill out your copyright notice in the Description page of Project Settings.

#include "BlastRadiusProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Character/BlastRadiusCharacter.h"
#include "Runtime/Engine/Classes/GameFramework/DamageType.h"
#include "Component/HealthComponent.h"
#include "Weapon/BlastRadiusSword.h"
#include "Runtime/Engine/Classes/GameFramework/DamageType.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystem.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "BlastRadiusExplosion.h"
#include "BlastRadiusGrenade.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABlastRadiusProjectile::ABlastRadiusProjectile()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComponent->InitSphereRadius(50.0f);
    CollisionComponent->BodyInstance.SetCollisionProfileName("Projectile");
    CollisionComponent->OnComponentHit.AddDynamic(this, &ABlastRadiusProjectile::OnHit);
    CollisionComponent->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
    CollisionComponent->CanCharacterStepUpOn = ECB_No;
    CollisionComponent->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
    RootComponent = CollisionComponent;

    //Setup Mesh (laser)
    ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    ProjectileMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    ProjectileMeshComponent->SetupAttachment(RootComponent);

    // Setup Projectile Movement Component
    // ProjectileMovementComponent is used to govern this projectile's movement
    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
    ProjectileMovementComponent->UpdatedComponent = CollisionComponent;
    ProjectileMovementComponent->InitialSpeed = 3000.f;
    ProjectileMovementComponent->MaxSpeed = 3000.f;
    ProjectileMovementComponent->bRotationFollowsVelocity = true;
    ProjectileMovementComponent->bShouldBounce = true;
    ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
    ProjectileMovementComponent->Bounciness = 1.0f;
    ProjectileMovementComponent->Friction = 0.0f;
    ProjectileMovementComponent->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::Z);

    // Template for HealthComponent->TakeDamage() parameter.
    DamageType = UDamageType::StaticClass();

    // Laser's lifespan.
    LifeSpan = 3.0f;

    // Damage
    LaserDamage = 50.0f;

    //Keeps track of amount of times the projectile bounced.
    BouncesMade = 0.0f;

    // Max amount of bounces until object is destroyed.
    MaxBounces = 4.0f;

    // Knock back Amount for collision with projectile. This might be Health percentage * 10 later.
    KnockbackFactor = 100.0f;

    // Blow back range for projectile on projectile explosion
    BlowBackRange = 25.0f;

    // Setup particle system component
    ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MyPSC1"));
    ParticleSystemComponent->SetupAttachment(RootComponent);

    // Setup projectile particle FX
    ProjectileFX = CreateDefaultSubobject<UParticleSystem>(TEXT("Projectile Particles"));
    ProjectileDestroyFX = CreateDefaultSubobject<UParticleSystem>(TEXT("Projectile Destroyed Particles"));

    // Enable replication
    SetReplicates(true);
    SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ABlastRadiusProjectile::BeginPlay()
{
    Super::BeginPlay();

    GetWorld()->GetTimerManager().SetTimer(SpawnTimer,
        this,
        &ABlastRadiusProjectile::DestroySelf,
        LifeSpan, true);

    if (ProjectileFX)
    {
        ParticleSystemComponent->SetTemplate(ProjectileFX);
    }
}

// Called every frame
void ABlastRadiusProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}


void ABlastRadiusProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    //Increment bounce when hitting walls until bounce limit is hit.
    BouncesMade++;
    if (BouncesMade <= MaxBounces)
    {
        LaserDamage += BouncesMade * 3;
        if (ProjectileDestroyFX)
        {
            //Spawn ParticleSystem using GamePlayStatics
            UGameplayStatics::SpawnEmitterAtLocation(this, ProjectileDestroyFX, GetActorLocation());
            //OR Spawn Particle using UParticleSystemComponent
            ParticleSystemComponent->SetTemplate(ProjectileDestroyFX);
        } 
    }
    else
        DestroySelf();


    if (OtherActor != nullptr && OtherComp != nullptr)
    {
        // Collision Response between projectile & character.
        ABlastRadiusCharacter* OtherCharacter = Cast<ABlastRadiusCharacter>(OtherActor);
        if (OtherCharacter != nullptr)
        {
            FVector projectileVel = GetVelocity();
            //Calling TakeDamage on the otherActor's HealthComponent. 
            const UDamageType* Laser_DamageType = Cast<UDamageType>(UDamageType::StaticClass());
            OtherCharacter->GetHealthComponent()->TakeDamage(LaserDamage, Laser_DamageType, OtherCharacter->GetInstigatorController(), GetOwner(), projectileVel);

            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "OtherCharacter Damage % - " + FString::SanitizeFloat(OtherCharacter->GetHealthComponent()->GetCurrentHealth())); // DEBUG

            if (ProjectileDestroyFX)
            {
                UGameplayStatics::SpawnEmitterAtLocation(this, ProjectileDestroyFX, GetActorLocation());
                ParticleSystemComponent->SetTemplate(ProjectileDestroyFX);
            }
            DestroySelf();
        }

        // Collision response between projectiles.
        ABlastRadiusProjectile* OtherProjectile = Cast<ABlastRadiusProjectile>(OtherActor);
        if (OtherProjectile != nullptr)
        {
            // Get a list of player actors.
            TArray<AActor*>CharacterActors;
            UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABlastRadiusCharacter::StaticClass(), CharacterActors);

            if (ExplosionClass)
            {
                UWorld* World = GetWorld();
                if (World)
                {
                    FActorSpawnParameters SpawnParams;
                    SpawnParams.Owner = this;
                    SpawnParams.Instigator = Instigator;
                    // Spawn the projectile at the muzzle.
                    World->SpawnActor<ABlastRadiusExplosion>(ExplosionClass, OtherProjectile->GetActorLocation(), GetActorRotation(), SpawnParams);
                }
            }

            if (ProjectileDestroyFX)
            {
                UGameplayStatics::SpawnEmitterAtLocation(this, ProjectileDestroyFX, GetActorLocation());
                ParticleSystemComponent->SetTemplate(ProjectileDestroyFX);
            }

            DestroySelf();
        }
    }
}

void ABlastRadiusProjectile::FireInDirection(const FVector& ShootDirection)
{
    ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}

void ABlastRadiusProjectile::DestroySelf()
{
    this->Destroy();
}

void ABlastRadiusProjectile::FlipVelocity()
{
    FVector NewVelocity;
    NewVelocity = ProjectileMovementComponent->Velocity;
    NewVelocity *= (FVector(-1.0, -1.0, 1.0));

    ProjectileMovementComponent->Velocity = NewVelocity;
}
