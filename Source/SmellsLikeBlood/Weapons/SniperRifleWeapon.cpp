//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#include "SniperRifleWeapon.h"
#include "SmellsLikeBlood/CommonObjects/PickupWeapon.h"

ASniperRifleWeapon::ASniperRifleWeapon()
{
    DefaultEnableSniperTimer = EnableSniperTimer;
    bCanShot                 = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ASniperRifleWeapon::FireWeapon()
{
    if (bCanShot)
    {
        bCanShot = false;
        Super::FireWeapon();
        GetWorldTimerManager().SetTimer(EnableSniper_TimerHandle, this, &ASniperRifleWeapon::EnableSniperShot, EnableSniperTimer, false);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ASniperRifleWeapon::StopFireWeapon()
{
    Super::StopFireWeapon();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ASniperRifleWeapon::DestroyThisWeapon()
{
    FActorSpawnParameters ActorSpawnParams;
    ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    GetWorld()->SpawnActor<APickupWeapon>(SpawnWeaponOnDestroy, GetActorTransform(), ActorSpawnParams);
    Destroy(true);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ASniperRifleWeapon::SetFasterBulletOnSlowTime(const bool& bOnSlowTime, const float& NewTimeDilation)
{
    Super::SetFasterBulletOnSlowTime(bOnSlowTime, NewTimeDilation);
    if (bOnSlowTime)
    {
        EnableSniperTimer = SlowEnableSniperTimer;
    }
    else
    {
        EnableSniperTimer = DefaultEnableSniperTimer;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ASniperRifleWeapon::BeginPlay()
{
    Super::BeginPlay();
    WeaponBeginPlay();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ASniperRifleWeapon::WeaponBeginPlay()
{
    Super::WeaponBeginPlay();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ASniperRifleWeapon::EnableSniperShot()
{
    bCanShot = true;
    GetWorldTimerManager().ClearTimer(EnableSniper_TimerHandle);
}