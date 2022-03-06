//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#include "BaseWeapon.h"

#include "Camera/CameraComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "Particles/ParticleSystemComponent.h"

#include "SmellsLikeBlood/Projectiles/BaseProjectile.h"
#include "SmellsLikeBlood/SmellsLikeBlood.h"

#include "Sound/SoundCue.h"

static FName NAME_WeaponSpawnSocket("MuzzleFlash");

//------------------------------------------------------------------------------------------------------------------------------------------
ABaseWeapon::ABaseWeapon()
{
    PrimaryActorTick.bCanEverTick = false;

    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponSkeletalMesh"));

    ShootLength  = 10000.0f;
    MinDamage    = 8.0f;
    MaxDamage    = 10.0f;
    CriticChance = 20.0f;
    ZoomFOV      = 45.0f;

    CooldownWeaponSmoke = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("CooldownSmoke"));
    CooldownWeaponSmoke->SetupAttachment(WeaponMesh);
    CooldownWeaponSmoke->bAutoActivate = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseWeapon::WeaponBeginPlay()
{
    ActualWeaponHeat      = 0.0f;
    InitialCoolDownAmount = CoolDownAmount;
    StartCoolDownWeapon();
}

//-----------------------------------------------------------------------------------------------------------------------------------------
void ABaseWeapon::FireWeapon()
{
    if (!bWeaponIsOverHeated)
    {
        WeaponMesh->PlayAnimation(OnFireAnimation, false);
        const FTransform SpawnTransform = CalculateShootInfo();

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        SpawnParams.Instigator                     = this->GetInstigator();
        SpawnParams.bNoFail                        = true;

        ABaseProjectile* SpawnedProjectile = GetWorld()->SpawnActor<ABaseProjectile>(ProjectileClass, SpawnTransform, SpawnParams);
        SpawnedProjectile->SetDamageInfo(MinDamage, MaxDamage, CriticChance);
        SpawnedProjectile->CustomTimeDilation = bPlayerOnSlowTime ? WeaponFasterTimeDilation : 1.0f;

        OverHeatWeapon();
        CooldownWeaponSmoke->Deactivate();

        if (bIsPlayerWeapon)
        {
            SpawnedProjectile->SpawnedByPlayer();
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseWeapon::StopFireWeapon()
{
    GetWorldTimerManager().SetTimer(EnableCoolDown_TimerHandle, this, &ABaseWeapon::EnableCoolDown, EnableCoolDownTimer, false);
    bIsShooting = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseWeapon::DestroyThisWeapon()
{
    /*Not used in BaseWeapon.*/
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseWeapon::ForceCoolDownWeapon()
{
    //If this weapon is over heated, we can't force cool down until we are in 50%.
    if (bWeaponIsOverHeated || bWeaponIsOverHeated && ActualWeaponHeat >= MaxOverHeatAmount / 2)
    {
        CoolDownAmount = InitialCoolDownAmount;
        CooldownWeaponSmoke->Deactivate();
    }
    else
    {
        CoolDownAmount = InitialCoolDownAmount * PowerCoolDown;
        bStopCoolDown  = false;

        if (ActualWeaponHeat > 0)
        {
            CooldownWeaponSmoke->Activate(true); 
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseWeapon::StopForceCoolDownWeapon()
{
    CoolDownAmount = InitialCoolDownAmount;
    CooldownWeaponSmoke->Deactivate();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseWeapon::OverHeatWeapon()
{
    bIsShooting   = true;
    bStopCoolDown = true;

    GetWorldTimerManager().ClearTimer(EnableCoolDown_TimerHandle);
    CheckCurrentHeat();
}

//------------------------------------------------------------------------------------------------------------------------------------------
float ABaseWeapon::ReturnHeatValue()
{
    return ActualWeaponHeat / MaxOverHeatAmount;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseWeapon::SetFasterBulletOnSlowTime(const bool& bOnSlowTime, const float& NewTimeDilation)
{
    bPlayerOnSlowTime        = bOnSlowTime;
    WeaponFasterTimeDilation = NewTimeDilation;
    this->CustomTimeDilation = bPlayerOnSlowTime ? WeaponFasterTimeDilation * 2.0f : 1.0f;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseWeapon::StartCoolDownWeapon()
{
    GetWorldTimerManager().SetTimer(CoolDown_TimerHandle, this, &ABaseWeapon::CoolDownWeapon, CoolDownRate, true, 0.0f);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseWeapon::CoolDownWeapon()
{
    if (bStopCoolDown)
    {
        return;
    }

    else if (!bIsShooting && ActualWeaponHeat > 0.0f)
    {
        ActualWeaponHeat -= CoolDownAmount;
        ActualWeaponHeat = FMath::Clamp(ActualWeaponHeat, 0.0f, MaxOverHeatAmount);
    }

    else if (ActualWeaponHeat <= 0.0f)
    {
        CooldownWeaponSmoke->Deactivate();
        bWeaponIsOverHeated = false;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseWeapon::CheckCurrentHeat()
{
    ActualWeaponHeat += OverheatAmount;
    ActualWeaponHeat = FMath::Clamp(ActualWeaponHeat, 0.0f, MaxOverHeatAmount);
    if (ActualWeaponHeat == MaxOverHeatAmount)
    {
        UGameplayStatics::PlaySound2D(GetWorld(), OverHeatShotSound);
        bWeaponIsOverHeated = true;
        bIsShooting         = false;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseWeapon::EnableCoolDown()
{
    bStopCoolDown = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------
FTransform ABaseWeapon::CalculateShootInfo()
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

    ReturnTransform = FTransform(TransformRotator, WeaponSocketLocation, FVector(1.0f, 1.0f, 1.0f));

    return ReturnTransform;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseWeapon::SetCamera(class UCameraComponent* Camera)
{
    PlayerCamera = Camera;
}