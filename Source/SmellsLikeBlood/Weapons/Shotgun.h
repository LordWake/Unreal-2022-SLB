//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "SmellsLikeBlood/SmellsLikeBlood.h"
#include "Shotgun.generated.h"

UCLASS()
class SMELLSLIKEBLOOD_API AShotgun : public ABaseWeapon
{
    GENERATED_BODY()

public:

    //*******************************************************************************************************************
    //                                          CONSTRUCTOR && PUBLIC VARIABLES                                         *
    //*******************************************************************************************************************

    //Constructor
    AShotgun();

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

    float DefaultEnableShotgunTimer;

    UPROPERTY(EditDefaultsOnly, Category = "Shotgun Data")
    float EnableShotgunTimer = 1.0f;
    UPROPERTY(EditDefaultsOnly, Category = "Shotgun Data")
    float ShotgunSpread      = 0.1f;
    UPROPERTY(EditDefaultsOnly, Category = "Shotgun Data")
    float SlowShotgunTimer   = 0.5f;

    UPROPERTY(EditDefaultsOnly, Category = "Shotgun Data")
    int32 BulletsPerShot = 8;

    //*******************************************************************************************************************
    //                                          PROTECTED FUNCTIONS                                                     *
    //*******************************************************************************************************************

    virtual void BeginPlay() override;
    virtual void WeaponBeginPlay() override;
    virtual void OverHeatWeapon() override;

    /*Enables shotgun again after "X" of fire.*/
    UFUNCTION()
    void EnableShotgunShot();

private:

    //*******************************************************************************************************************
    //                                          PRIVATE VARIABLES                                                       *
    //*******************************************************************************************************************

    FTimerHandle EnableShotgun_TimerHandle;
    bool bCanShot;

    //*******************************************************************************************************************
    //                                          PRIVATE FUNCTIONS                                                       *
    //*******************************************************************************************************************

    /*Spawns shotgun bullet.*/
    void ShotGunFireBurst();
    /*Randomize shotgun bullets position.*/
    FTransform CalculateProjectileInfo();
};