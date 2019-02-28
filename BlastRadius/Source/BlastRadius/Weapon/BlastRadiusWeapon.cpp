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

// Sets default values
ABlastRadiusWeapon::ABlastRadiusWeapon()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;


    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("WeaponMesh");
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RootComponent = MeshComponent;

    MuzzleArrow = CreateDefaultSubobject<UArrowComponent>("WeaponArrow");
    //MuzzleArrow->AddLocalOffset(MuzzleDirectionOffSet);
    MuzzleArrow->SetupAttachment(RootComponent);


    //MagnitudeOffSet = 50.0f;
    //if (GetOwner() != nullptr)
    //{
    //    SetActorRotation(GetOwner()->GetActorRotation());
    //    MuzzleDirectionOffSet = GetActorRotation().Vector().GetSafeNormal();
    //    MuzzleDirectionOffSet *= MagnitudeOffSet;
    //    MuzzleLocation = GetActorLocation() + MuzzleDirectionOffSet;
    //}

    static ConstructorHelpers::FObjectFinder<UParticleSystem> PS(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Explosion'"));
    if (PS.Succeeded())
    {
        ProjectileFX = PS.Object;
    }
    PSC = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MyPSC"));
    //PSC->SetTemplate(PS.Object); //If you want it to Spawn on Creation, could go to BeginPlay too
    PSC->SetupAttachment(RootComponent);
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

    ////Add this somewhere here to spawn particles.
    //if (ProjectileFX)
    //{
    //    //Spawn ParticleSystem using GamePlayStatics
    //    //UGameplayStatics::SpawnEmitterAtLocation(this, ProjectileFX, GetActorLocation());
    //    //OR Spawn Particle using UParticleSystemComponent
    //    PSC->SetTemplate(ProjectileFX);
    //    //ProjectileSprite->bHiddenInGame = true;
    //    //ProjectileSprite->SetVisibility(false);
    //}

}

void ABlastRadiusWeapon::Attach(class ABlastRadiusCharacter* Character)
{
    verify(Character != nullptr && "Weapon's attach function called with null character!");

    /* Set owner of this weapon.*/
    SetOwner(Character);
    SetActorHiddenInGame(false);

    // Disable weapon's physics.
    //CALL SetActorEnableCollision() to false
    SetActorEnableCollision(false);
    //CALL SetSimulatePhysics() in the Primitive and pass in false, disabling physics


    // Attach weapon to the character's mesh.
    //CALL AttachToComponent() and pass in (Character->GetSkeletalMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "WeaponSocket") <-- We are attaching this Actor to the Characters Skeletal Mesh at the WeaponSocket
    AttachToComponent(Character->GetSkeletalMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "GunSocket");

}
