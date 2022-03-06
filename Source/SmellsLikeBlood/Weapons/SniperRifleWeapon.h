//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "SmellsLikeBlood/SmellsLikeBlood.h"
#include "SniperRifleWeapon.generated.h"

UCLASS()
class SMELLSLIKEBLOOD_API ASniperRifleWeapon : public ABaseWeapon
{
    GENERATED_BODY()

public:

    //*******************************************************************************************************************
    //                                         CONSTRUCTOR && PUBLIC VARIABLES                                          *
    //*******************************************************************************************************************

    //Constructor
    ASniperRifleWeapon();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Type of Weapon")
    EAllWeaponTypes ThisWeaponType;

    //*******************************************************************************************************************
    //                                          PUBLIC FUNCTIONS                                                        *
    //*******************************************************************************************************************

    virtual void FireWeapon() override;
    virtual void StopFireWeapon() override;
    virtual void DestroyThisWeapon() override;
    virtual void SetFasterBulletOnSlowTime(const bool& bOnSlowTime, const float& NewTimeDilation) override;

protected:

    //*******************************************************************************************************************
    //                                          PROTECTED VARIABLES                                                     *
    //*******************************************************************************************************************

    float DefaultEnableSniperTimer;

    UPROPERTY(EditDefaultsOnly, Category = "Sniper Data")
    float EnableSniperTimer = 2.5f;
    UPROPERTY(EditDefaultsOnly, Category = "Sniper Data")
    float SlowEnableSniperTimer;

    //*******************************************************************************************************************
    //                                          PROTECTED FUNCTIONS                                                     *
    //*******************************************************************************************************************

    virtual void BeginPlay() override;
    virtual void WeaponBeginPlay() override;

    /*Enables sniper again after "X" of fire.*/
    UFUNCTION()
    void EnableSniperShot();

private:

    //*******************************************************************************************************************
    //                                          PRIVATE VARIABLES                                                       *
    //*******************************************************************************************************************

    FTimerHandle EnableSniper_TimerHandle;
    bool bCanShot;
};