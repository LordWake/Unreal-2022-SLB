//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EAllWeaponTypes : uint8
{
    None,
    AssaultRifleWeapon,
    LegendaryAssaultRifle,
    ShotgunWeapon,
    LegendaryShotgun,
    SniperRifleWeapon,
    LegendarySniperRifle
};

UENUM(BlueprintType)
enum class ERumbleFeedBack : uint8
{
    LowHealthFeedBack,
    RifleFeedBack,
    ShotgunFeedBack,
    SniperFeedBack,
    MeleeFeedBack,
    TakeDamageFeedBack
};

#define WEAPON_TRACE    ECC_GameTraceChannel1