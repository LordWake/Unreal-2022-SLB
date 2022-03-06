//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#include "AssaultRifle.h"
#include "SmellsLikeBlood/CommonObjects/PickupWeapon.h"

AAssaultRifle::AAssaultRifle()
{
    bIsFiring = false;

    FireRate              = 0.15f;
    HorizontalRifleRecoil = 0.05f;
    VerticalRifleRecoil   = 0.01f;
    DefaultFireRate       = FireRate;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AAssaultRifle::BeginPlay()
{
    Super::BeginPlay();
    WeaponBeginPlay();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AAssaultRifle::WeaponBeginPlay()
{
    Super::WeaponBeginPlay();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AAssaultRifle::FireWeapon()
{
    if (!bIsFiring)
    {
        bIsFiring = true;
        GetWorldTimerManager().SetTimer(TimerHandle_AutomaticFire, this, &AAssaultRifle::AutomaticFire, FireRate, true);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AAssaultRifle::StopFireWeapon()
{
    bIsFiring = false;
    GetWorldTimerManager().ClearTimer(TimerHandle_AutomaticFire);

    Super::StopFireWeapon();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AAssaultRifle::DestroyThisWeapon()
{
    FActorSpawnParameters ActorSpawnParams;
    ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    GetWorld()->SpawnActor<APickupWeapon>(SpawnWeaponOnDestroy, GetActorTransform(), ActorSpawnParams);
    Destroy(true);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AAssaultRifle::SetFasterBulletOnSlowTime(const bool& bOnSlowTime, const float& NewTimeDilation)
{
    Super::SetFasterBulletOnSlowTime(bOnSlowTime, NewTimeDilation);
    if (bOnSlowTime)
    {
        FireRate = SlowFireRate;
    }
    else
    {
        FireRate = DefaultFireRate;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AAssaultRifle::AutomaticFire()
{
    if (bIsFiring)
    {
        Super::FireWeapon();
        RecoilRifleWeapon();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
float AAssaultRifle::HorizontalRecoilRandomized()
{
    float randValue   = FMath::RandRange(0.0f, 100.0f);
    float returnValue = 0.0f;

    if (randValue < 50.0f)
    {
        returnValue = HorizontalRifleRecoil;
    }
    else
    {
        returnValue = HorizontalRifleRecoil * -1.0f;
    }

    return returnValue;
}