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

    //Blow back range for projectile on projectile explosion
    m_BlowBackRange = 25.0f;


    /*
    In Blueprint editor:
    For a multiplayer game, We'll need to uncheck "Initial Velocity in Local Space"
    in the "MovementComp" Component in order for this projectile to replicate correctly over a server.
    */


    ProjectileFX = CreateDefaultSubobject<UParticleSystem>(TEXT("Projectile Particles"));
    ProjectileDestroyFX = CreateDefaultSubobject<UParticleSystem>(TEXT("Projectile Destroyed Particles"));

    PSC1 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MyPSC1"));
    PSC1->SetupAttachment(RootComponent);


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
        m_LifeSpan, true);

    if (ProjectileFX)
    {
        PSC1->SetTemplate(ProjectileFX);
    }
}

// Called every frame
void ABlastRadiusProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}


void ABlastRadiusProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

    if (OtherActor != nullptr && OtherComp != nullptr)
    {
        // Collision Response between projectile & character.
        //ABlastRadiusCharacter* ThisCharacter = Cast<ABlastRadiusCharacter>(GetOwner());
        ABlastRadiusCharacter* OtherCharacter = Cast<ABlastRadiusCharacter>(OtherActor);
        if (OtherCharacter != nullptr)
        {
            //Calling TakeDamage on the otherActor's HealthComponent. 
            const UDamageType* Laser_DamageType = Cast<UDamageType>(UDamageType::StaticClass());
            OtherCharacter->GetHealthComponent()->TakeDamage(m_LaserDamage, Laser_DamageType, OtherCharacter->GetInstigatorController(), GetOwner(), GetVelocity());
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "OtherCharacter Damage % - " + FString::SanitizeFloat(OtherCharacter->GetHealthComponent()->GetCurrentHealth())); // DEBUG

            if (ProjectileDestroyFX)
            {
                UGameplayStatics::SpawnEmitterAtLocation(this, ProjectileDestroyFX, GetActorLocation());
                PSC1->SetTemplate(ProjectileDestroyFX);
            }
            DestroySelf();
        }


        // Projectile 2 Projectile logic. - Remi
        //Get a list of characters.
        //Calculate math
        //  Damage = m_LaserDamage / m_MaxBounceAmount;  this prevents the maximum damage build up from being higher than a base attack.
        //  blowback direction = direction * velocity
        //  blowback vector = -direction * m_MaxBounceAmount
        //Check if the character actors are within range.
        //apply damage to characters in range

        // Collision response between projectiles.
        ABlastRadiusProjectile* OtherProjectile = Cast<ABlastRadiusProjectile>(OtherActor);
        if (OtherProjectile != nullptr)
        {
            // Get a list of player actors.
            TArray<AActor*>CharacterActors;
            UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABlastRadiusCharacter::StaticClass(), CharacterActors);

            // Calculate the math for appropriate reponse.
            FVector difference = OtherProjectile->GetActorLocation() - this->GetActorLocation();
            FVector direction = difference.GetSafeNormal();
            float distance = difference.Size();

            // Cycle through all players.
            for (auto Actor : CharacterActors)
            {
                // Calculate direction & distance to impact for each character(actor)
                FVector directionToImpactLocation = FVector(difference - Actor->GetActorLocation()).GetSafeNormal();
                float playerToImpactDistance = FVector(this->GetActorLocation() - Actor->GetActorLocation()).Size();

                FVector BlowBackVector = directionToImpactLocation * m_MaxBounceAmount;

                // If player is in range.
                if (playerToImpactDistance > m_BlowBackRange)
                {
                    // Call TakeDamage on all characters within range's HealthComponent. 
                    ABlastRadiusCharacter* Character = Cast<ABlastRadiusCharacter>(Actor);
                    const UDamageType* Laser_DamageType = Cast<UDamageType>(UDamageType::StaticClass());
                    Character->GetHealthComponent()->TakeDamage(m_LaserDamage / m_MaxBounceAmount, Laser_DamageType, Character->GetInstigatorController(), GetOwner(), BlowBackVector * GetVelocity());
                    //GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "BlowBackDamage % - " + FString::SanitizeFloat(Character->GetHealthComponent()->GetCurrentHealth())); // DEBUG
                }
            }

            if (ProjectileDestroyFX)
            {
                UGameplayStatics::SpawnEmitterAtLocation(this, ProjectileDestroyFX, GetActorLocation());
                PSC1->SetTemplate(ProjectileDestroyFX);
            }

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
            if (ProjectileDestroyFX)
            {
                //Spawn ParticleSystem using GamePlayStatics
                UGameplayStatics::SpawnEmitterAtLocation(this, ProjectileDestroyFX, GetActorLocation());
                //OR Spawn Particle using UParticleSystemComponent
                PSC1->SetTemplate(ProjectileDestroyFX);
                //ProjectileSprite->bHiddenInGame = true;
                //ProjectileSprite->SetVisibility(false);
            }
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

void ABlastRadiusProjectile::FlipVelocity()
{
    FVector NewVelocity;
    NewVelocity = ProjectileMovementComp->Velocity;
    NewVelocity *= (FVector(-1.0, -1.0, 1.0));

    ProjectileMovementComp->Velocity = NewVelocity;
}
