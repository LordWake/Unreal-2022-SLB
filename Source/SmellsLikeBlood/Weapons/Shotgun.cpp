//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#include "Shotgun.h"

#include "Camera/CameraComponent.h" 

#include "Kismet/KismetMathLibrary.h"

#include "SmellsLikeBlood/CommonObjects/PickupWeapon.h"
#include "SmellsLikeBlood/Projectiles/BaseProjectile.h"

static FName NAME_WeaponSpawnSocket("MuzzleFlash");

//-----------------------------------------------------------------------------------------------------------------------------------------
AShotgun::AShotgun()
{
    DefaultEnableShotgunTimer = EnableShotgunTimer;
    bCanShot                  = true;
}

//-----------------------------------------------------------------------------------------------------------------------------------------
void AShotgun::FireWeapon()
{
    if((bCanShot) && (!bWeaponIsOverHeated)) 
    {
        WeaponMesh->PlayAnimation(OnFireAnimation, false);
        bCanShot = false;

        for (int32 i = 0; i < BulletsPerShot; ++i)
        {
            ShotGunFireBurst();
        }

        OverHeatWeapon();
        GetWorldTimerManager().SetTimer(EnableShotgun_TimerHandle, this, &AShotgun::EnableShotgunShot, EnableShotgunTimer, false);
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------
void AShotgun::StopFireWeapon()
{
    Super::StopFireWeapon();
}

//-----------------------------------------------------------------------------------------------------------------------------------------
void AShotgun::DestroyThisWeapon()
{
    FActorSpawnParameters ActorSpawnParams;
    ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    GetWorld()->SpawnActor<APickupWeapon>(SpawnWeaponOnDestroy, GetActorTransform(), ActorSpawnParams);
    Destroy(true);
}

//-----------------------------------------------------------------------------------------------------------------------------------------
void AShotgun::SetFasterBulletOnSlowTime(const bool& bOnSlowTime, const float& NewTimeDilation)
{
    Super::SetFasterBulletOnSlowTime(bOnSlowTime, NewTimeDilation);
    if (bOnSlowTime)
    {
        EnableShotgunTimer = SlowShotgunTimer;
    }
    else
    {
        EnableShotgunTimer = DefaultEnableShotgunTimer;
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------
void AShotgun::BeginPlay()
{
    Super::BeginPlay();
    WeaponBeginPlay();
}

//-----------------------------------------------------------------------------------------------------------------------------------------
void AShotgun::WeaponBeginPlay()
{
    Super::WeaponBeginPlay();
}

//-----------------------------------------------------------------------------------------------------------------------------------------
void AShotgun::OverHeatWeapon()
{
    Super::OverHeatWeapon();
}

//-----------------------------------------------------------------------------------------------------------------------------------------
void AShotgun::EnableShotgunShot()
{
    bCanShot = true;
    GetWorldTimerManager().ClearTimer(EnableShotgun_TimerHandle);
}

//-----------------------------------------------------------------------------------------------------------------------------------------
void AShotgun::ShotGunFireBurst()
{
    FTransform ProjectileSpawnInfo = CalculateProjectileInfo();

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    SpawnParams.Instigator                     = this->GetInstigator();
    SpawnParams.bNoFail                        = true;

    ABaseProjectile* SpawnedProjectile = GetWorld()->SpawnActor<ABaseProjectile>(ProjectileClass, ProjectileSpawnInfo, SpawnParams);

    SpawnedProjectile->SetDamageInfo(MinDamage, MaxDamage, CriticChance);
    SpawnedProjectile->CustomTimeDilation = bPlayerOnSlowTime ? WeaponFasterTimeDilation : 1.0f;

    if (bIsPlayerWeapon)
    { 
        SpawnedProjectile->SpawnedByPlayer(); 
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------
FTransform AShotgun::CalculateProjectileInfo()
{
    FTransform ReturnTransform;
    FHitResult HitResult;

    if (PlayerCamera == nullptr)
    {
        return ReturnTransform;
    }

    const FVector StartTrace           = PlayerCamera->GetComponentLocation();
    const FVector EndTrace             = (PlayerCamera->GetForwardVector() * ShootLength) + StartTrace; 
    const FVector WeaponSocketLocation = WeaponMesh->GetSocketLocation(NAME_WeaponSpawnSocket);

    FRotator TransformRotator;

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    QueryParams.bTraceComplex = false;

    const bool LineTrace = GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, WEAPON_TRACE, QueryParams);

    if (LineTrace)
    {
        const FVector TargetRotation = HitResult.ImpactPoint;
        TransformRotator = UKismetMathLibrary::FindLookAtRotation(WeaponSocketLocation, TargetRotation);
    }
    else
    {
        TransformRotator = UKismetMathLibrary::FindLookAtRotation(WeaponSocketLocation, EndTrace);
    }

    const FVector RandomUnit = UKismetMathLibrary::RandomUnitVectorInConeInRadians(UKismetMathLibrary::GetForwardVector(TransformRotator), ShotgunSpread);
    ReturnTransform          = FTransform(UKismetMathLibrary::Conv_VectorToRotator(RandomUnit), WeaponSocketLocation, FVector(1.0f, 1.0f, 1.0f));

    return ReturnTransform;
}