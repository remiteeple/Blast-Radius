// Fill out your copyright notice in the Description page of Project Settings.

#include "BlastRadiusWeapon.h"
#include "Weapon/BlastRadiusProjectile.h"
#include "Character/BlastRadiusCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystem.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"

// Sets default values
ABlastRadiusWeapon::ABlastRadiusWeapon()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    /* Setup Mesh Component */
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("WeaponMesh");
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RootComponent = MeshComponent;

    /* Setup Arrow Component (Muzzle) */
    MuzzleArrow = CreateDefaultSubobject<UArrowComponent>("WeaponArrow");
    MuzzleArrow->SetupAttachment(RootComponent);

    /* Setup Projectile Particle System */
    ProjectileFX = CreateDefaultSubobject<UParticleSystem>(TEXT("Firing Particles"));

    /* Setup Audio Component */
    AudioComponent = CreateDefaultSubobject<UAudioComponent>("WeaponSound");
    AudioComponent->SetupAttachment(RootComponent);
    AudioComponent->bAutoActivate = false;
    AudioComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

    /* Setup Projectile System Component */
    ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MyPSC"));
    //PSC->SetTemplate(PS.Object); //If you want it to Spawn on Creation, could go to BeginPlay too
    ParticleSystemComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABlastRadiusWeapon::BeginPlay()
{
    Super::BeginPlay();

}

// Called every frame
void ABlastRadiusWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void ABlastRadiusWeapon::Fire()
{
    // Attempt to fire a projectile.
    if (ProjectileClass)
    {
        UWorld* World = GetWorld();
        if (World)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this->GetOwner();
            SpawnParams.Instigator = Instigator;
            // Spawn the projectile at the muzzle.
            ABlastRadiusProjectile* Projectile = World->SpawnActor<ABlastRadiusProjectile>(ProjectileClass, MuzzleArrow->GetComponentLocation(), GetOwner()->GetActorRotation(), SpawnParams);
            if (Projectile)
            {
                //Setting the projectile's Owner to this so we don't collide with it during OnHit.
                ACharacter* myCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

                Projectile->SetOwner(myCharacter);

                // Set the projectile's initial trajectory.
                FVector LaunchDirection = GetOwner()->GetActorRotation().Vector();
                Projectile->FireInDirection(LaunchDirection);

            }
        }
    }

    //Add this somewhere here to spawn particles.
    if (ProjectileFX)
    {
        UGameplayStatics::SpawnEmitterAtLocation(this, ProjectileFX, MuzzleArrow->GetComponentLocation());
        ParticleSystemComponent->SetTemplate(ProjectileFX);
        ParticleSystemComponent->SecondsBeforeInactive = 0.5;

        //Play the sound for shooting
        AudioComponent->SetSound(ShootingSound);
        AudioComponent->Play();
    }
}

void ABlastRadiusWeapon::Attach(class ABlastRadiusCharacter* Character)
{
    verify(Character != nullptr && "Weapon's attach function called with null character!");

    /* Set owner of this weapon.*/
    SetOwner(Character);
    SetActorHiddenInGame(false);

    // Disable weapon's physics.
    SetActorEnableCollision(false);

    // Attach weapon to the character's gun socket.
    AttachToComponent(Character->GetSkeletalMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "GunSocket");
}
