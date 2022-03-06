//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "SmellsLikeBlood/SmellsLikeBlood.h"
#include "AssaultRifle.generated.h"

UCLASS()
class SMELLSLIKEBLOOD_API AAssaultRifle : public ABaseWeapon
{
    GENERATED_BODY()

public:

    //*******************************************************************************************************************
    //                                         CONSTRUCTOR && PUBLIC VARIABLES                                          *
    //*******************************************************************************************************************

    //Constructor
    AAssaultRifle();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Type of Weapon")
    EAllWeaponTypes ThisWeaponType;

    //*******************************************************************************************************************
    //                                          PUBLIC FUNCTIONS                                                        *
    //*******************************************************************************************************************

    /*Sets a timer to make to call automatic fire.*/
    virtual void FireWeapon() override;
    /*Stops weapon fire.*/
    virtual void StopFireWeapon() override;
    /*Destroy this weapon and spawn a pickup weapon of this type.*/
    virtual void DestroyThisWeapon() override;
    /*Increase bullet speed when time dilation is slower because of slow time feature.*/
    virtual void SetFasterBulletOnSlowTime(const bool& bOnSlowTime, const float& NewTimeDilation) override;

    /*Calls the FireWeapon function from parent.*/
    UFUNCTION()
    void AutomaticFire();

protected:

    //*******************************************************************************************************************
    //                                          PROTECTED VARIABLES                                                     *
    //*******************************************************************************************************************

    float DefaultFireRate;

    UPROPERTY(EditDefaultsOnly, Category  = "WeaponData")
    float HorizontalRifleRecoil;
    UPROPERTY(EditDefaultsOnly, Category  = "WeaponData")
    float FireRate;
    UPROPERTY(BlueprintReadOnly, Category = "WeaponData")
    float VerticalRifleRecoil;
    UPROPERTY(EditDefaultsOnly, Category  = "WeaponData")
    float SlowFireRate;

    UPROPERTY(VisibleDefaultsOnly, Category = "WeaponData")
    bool bIsFiring;

    FTimerHandle TimerHandle_AutomaticFire;

    //*******************************************************************************************************************
    //                                          PROTECTED FUNCTIONS                                                     *
    //*******************************************************************************************************************

    virtual void BeginPlay() override;
    virtual void WeaponBeginPlay() override;

    //Adds pitch/yaw input to the controller by using a TimeLine to make recoil effect.
    UFUNCTION(BlueprintImplementableEvent)
    void RecoilRifleWeapon();

    //Randomize horizontal recoil to left or right.
    UFUNCTION(BlueprintPure)
    float HorizontalRecoilRandomized();
};