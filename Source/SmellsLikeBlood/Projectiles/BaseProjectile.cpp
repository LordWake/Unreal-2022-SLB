//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#include "BaseProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "SmellsLikeBlood/Player/TPS_PlayerCharacter.h"

ABaseProjectile::ABaseProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    Bullet = CreateDefaultSubobject<USphereComponent>(TEXT("Bullet"));
    Bullet->SetSphereRadius(1.0f);
    Bullet->SetLinearDamping(0.0f);
    Bullet->SetEnableGravity(false);
    SetRootComponent(Bullet);

    ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystem"));
    ParticleSystem->SetupAttachment(Bullet);
    ParticleSystem->SetRelativeLocation(FVector(-45.0f, 0.0f, 0.0f));
    ParticleSystem->SetRelativeScale3D(FVector(0.5F, 0.5F, 0.5F));

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->ProjectileGravityScale = 0.0f;
    ProjectileMovement->InitialSpeed           = 5000.0f;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseProjectile::SetDamageInfo(float MinDamage, float MaxDamage, float CriticChance)
{
    _MinDamage    = MinDamage;
    _MaxDamage    = MaxDamage;
    _CriticChance = CriticChance;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseProjectile::SpawnedByPlayer()
{
    if (ATPS_PlayerCharacter* MyCharacter = Cast<ATPS_PlayerCharacter>(this->GetInstigator()))
    {
        MyCharacter->BulletSpawnedFeedBack();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseProjectile::BeginPlay()
{
    Super::BeginPlay();
    this->SetLifeSpan(10.0f);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseProjectile::BulletHit(const FHitResult& BulletHitResult)
{
    const FTransform& ParticleSpawnTransofrm = FTransform(GetActorRotation(), BulletHitResult.ImpactPoint, FVector(1.0f, 1.0f, 1.0f));

    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ImpactVFX, ParticleSpawnTransofrm, true, EPSCPoolMethod::AutoRelease, true);

    if (UKismetSystemLibrary::DoesImplementInterface(BulletHitResult.GetActor(), UDamageInterface::StaticClass()))
    {
        IDamageInterface::Execute_TakeDamage(BulletHitResult.GetActor(), _MinDamage, _MaxDamage, _CriticChance, BulletHitResult.BoneName, this->GetInstigator());
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), BodyImpactSound, GetActorLocation());
    }

    else
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, BulletHitResult.ImpactPoint);
    }

    Destroy(true);
}