//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+


#include "ExplosiveBarrel.h"

#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"

#include "SmellsLikeBlood/Enemies/ShooterEnemy.h"
#include "SmellsLikeBlood/Player/TPS_PlayerCharacter.h"
#include "SmellsLikeBlood/Projectiles/BaseProjectile.h"

#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

AExplosiveBarrel::AExplosiveBarrel()
{
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);

    BarrelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RedBarrel"));
    BarrelMesh->SetSimulatePhysics(true);
    BarrelMesh->SetGenerateOverlapEvents(true);
    BarrelMesh->SetupAttachment(RootComponent);

    FireVFX = CreateDefaultSubobject<UParticleSystemComponent>("FireParticle");
    FireVFX->SetupAttachment(BarrelMesh);
    FireVFX->SetRelativeScale3D(FVector(3.0f, 3.0f, 3.0f));
    FireVFX->SetRelativeLocation(FVector(0.0f, 0.0f, 220.0f));
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AExplosiveBarrel::BeginPlay()
{
    Super::BeginPlay();
    BarrelMesh->OnComponentHit.AddDynamic(this, &AExplosiveBarrel::OnBarrelHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AExplosiveBarrel::OnBarrelHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    if (bBarrelDestroyed)
    {
        return;
    }

    if (Cast<ABaseProjectile>(OtherActor))
    { 
        OnBarrelExplosion();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AExplosiveBarrel::OnBarrelExplosion()
{
    //Used to prevent infinite loop.
    if (bBarrelDestroyed)
    {
        return;
    }

    bBarrelDestroyed = true;
    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionVFX, GetActorLocation(), FRotator::ZeroRotator, FVector(1.f), true, EPSCPoolMethod::AutoRelease, true);
    UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionCue, GetActorLocation());

    //Destroy the mesh and break the DestructibleMesh after play a sound and spawn the explosion particle.
    FireVFX->DestroyComponent(false);
    BarrelMesh->DestroyComponent(false);
    OnMeshExplosion();

    //*******************************************************************************************************************
    //  From here and beyond, it will check for near barrels that should explode too using a SphereTraceMultiForObjects *
    //*******************************************************************************************************************

    TArray<TEnumAsByte<EObjectTypeQuery>> IncludeObjectTypes;
    IncludeObjectTypes.Add((EObjectTypeQuery)ECC_Pawn);
    IncludeObjectTypes.Add((EObjectTypeQuery)ECC_PhysicsBody);

    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(this);
    TArray<FHitResult> HitResults;

    UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), GetActorLocation(), GetActorLocation(), 250.0f, IncludeObjectTypes, true, ActorsToIgnore, EDrawDebugTrace::None, HitResults, true);

    for (const FHitResult& ThisHit : HitResults)
    {
        if (!ActorsToIgnore.Contains(ThisHit.GetActor()))
        {
            ActorsToIgnore.Add(ThisHit.GetActor());

            if (Cast<ATPS_PlayerCharacter>(ThisHit.GetActor()) || Cast<AShooterEnemy>(ThisHit.GetActor()))
            {
                const float CheckDamageValue = Cast<ATPS_PlayerCharacter>(ThisHit.GetActor()) ? ExplosiveBarrel_PlayerDamage : ExplosiveBarrel_EnemyDamage;
                IDamageInterface::Execute_TakeDamage(ThisHit.GetActor(), CheckDamageValue, CheckDamageValue, 0.0f, ThisHit.BoneName, this->GetInstigator());
            }

            else if (AExplosiveBarrel* OtherBarrel = Cast<AExplosiveBarrel>(ThisHit.GetActor()))
            {
                OtherBarrel->OnBarrelExplosion();
            }
        }
    }
}