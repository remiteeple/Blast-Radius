// Fill out your copyright notice in the Description page of Project Settings.

#include "BlastRadiusGrenade.h" 
#include "BlastRadiusExplosion.h"
#include "BlastRadiusProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Character/BlastRadiusCharacter.h"
#include "Runtime/Engine/Classes/GameFramework/DamageType.h"
#include "Component/HealthComponent.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystem.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Component/EnergyComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABlastRadiusGrenade::ABlastRadiusGrenade()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // Setup collision sphere
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
    CollisionComponent->InitSphereRadius(50.0f);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionComponent->CanCharacterStepUpOn = ECB_No;
    CollisionComponent->OnComponentHit.AddDynamic(this, &ABlastRadiusGrenade::OnHit);
    RootComponent = CollisionComponent;

    // Setup Mesh (grenade)
    GrenadeMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
    GrenadeMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GrenadeMeshComponent->SetupAttachment(RootComponent);

    // Setup Projectile Movement Component
    GrenadeMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Component"));
    GrenadeMovementComponent->UpdatedComponent = CollisionComponent;
    GrenadeMovementComponent->InitialSpeed = 1000.f;
    GrenadeMovementComponent->MaxSpeed = 3000.f;
    GrenadeMovementComponent->bRotationFollowsVelocity = true;
    GrenadeMovementComponent->bShouldBounce = true;
    GrenadeMovementComponent->ProjectileGravityScale = 2.0f;
    GrenadeMovementComponent->Bounciness = 1.0f;
    GrenadeMovementComponent->Friction = 1.5f;

    // Setup Audio Component
    AudioComponent = CreateDefaultSubobject<UAudioComponent>("WeaponSound");
    AudioComponent->SetupAttachment(RootComponent);
    AudioComponent->bAutoActivate = false;
    AudioComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

    // Template for HealthComponent->TakeDamage() parameter.
    DamageType = UDamageType::StaticClass();

    // Grenade Fuse.
    FuseTime = 3.0f;

    // Damage
    GrenadeDamage = 100.0f;

    // Knock back Amount for collision with projectile. This might be Health percentage * 10 later.
    KnockbackFactor = 150.0f;

    // Blow back range for projectile on projectile explosion
    BlowBackRange = 50.0f;

    /*
    In Blueprint editor:
    For a multiplayer game, We'll need to uncheck "Initial Velocity in Local Space"
    in the "MovementComp" Component in order for this projectile to replicate correctly over a server.
    */

    // Setup particle system component
    ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MyPSC1"));
    ParticleSystemComponent->SetupAttachment(RootComponent);

    // Setup projectile particle FX
    GrenadeFX = CreateDefaultSubobject<UParticleSystem>(TEXT("Grenade Particles"));
    GrenadeDestroyFX = CreateDefaultSubobject<UParticleSystem>(TEXT("Projectile Destroyed Particles"));

    // Enable replication
    SetReplicates(true);
    SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ABlastRadiusGrenade::BeginPlay()
{
    Super::BeginPlay();

    /* Set Time until explode */
    GetWorld()->GetTimerManager().SetTimer(SpawnTimer, this, &ABlastRadiusGrenade::Explode, FuseTime, false);

    /* Assign travel particle */
    if (GrenadeFX)
    {
        ParticleSystemComponent->SetTemplate(GrenadeFX);
    }
}

// Called every frame
void ABlastRadiusGrenade::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ABlastRadiusGrenade::LobInDirection(const FVector& ShootDirection)
{
    GrenadeMovementComponent->Velocity = ShootDirection * GrenadeMovementComponent->InitialSpeed;
}

void ABlastRadiusGrenade::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor != nullptr && OtherComp != nullptr)
    {
        // Collision Response between projectile & character.
        ABlastRadiusProjectile* OtherProjectile = Cast<ABlastRadiusProjectile>(OtherActor);
        if (OtherProjectile != nullptr)
        {
            // Destroy impacting projectile.
            OtherProjectile->Destroy();

            // Explode on impact with projectile.
            Explode();
        }

        // Collision Response between grenade & character.
        ABlastRadiusCharacter* OtherCharacter = Cast<ABlastRadiusCharacter>(OtherActor);
        if (OtherCharacter != nullptr)
        {
            // Explode on impact with character.
            Explode();
        }
    }
}

void ABlastRadiusGrenade::Explode()
{
    /* Explode */
    if (Role == ROLE_Authority)
        ServerExplode();
    else if (Role == ROLE_AutonomousProxy)
        ServerExplode();

    /* Play Sound */
    AudioComponent->SetSound(ExplodeSound);
    AudioComponent->Play();
}

void ABlastRadiusGrenade::ServerExplode_Implementation()
{
    // Spawn explosion.
    if (ExplosionClass != nullptr)
    {
        if (ExplosionClass)
        {
            // Set explosion params.
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            SpawnParams.Instigator = Instigator;
            // Spawn explosion at the grenade's location
            GetWorld()->SpawnActor<ABlastRadiusExplosion>(ExplosionClass, GetActorLocation(), GetActorRotation(), SpawnParams);
        }
    }

    // Mutlicast explosion effect.
    PlayExplosionEffect();

    // Remove grenade from world
    DestroySelf();
}

bool ABlastRadiusGrenade::ServerExplode_Validate()
{
    return true;
}

void ABlastRadiusGrenade::PlayExplosionEffect_Implementation()
{
    // Play grenade destruction particle system.
    if (GrenadeDestroyFX)
    {
        ParticleSystemComponent->SecondsBeforeInactive = 0.75;
        UGameplayStatics::SpawnEmitterAtLocation(this, GrenadeDestroyFX, GetActorLocation());
        ParticleSystemComponent->SetTemplate(GrenadeDestroyFX);
    }

    // Play the sound for exploding
    AudioComponent->SetSound(ExplodeSound);
    AudioComponent->Play();
}

void ABlastRadiusGrenade::DestroySelf()
{
    this->Destroy();
}

void ABlastRadiusGrenade::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ABlastRadiusGrenade, FuseTime);
}