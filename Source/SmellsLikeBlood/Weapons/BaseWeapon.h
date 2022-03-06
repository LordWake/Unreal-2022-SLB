//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

UCLASS(Blueprintable)
class SMELLSLIKEBLOOD_API ABaseWeapon : public AActor
{
    GENERATED_BODY()

    friend class ATPS_PlayerCharacter;

public:

    //*******************************************************************************************************************
    //                                         CONSTRUCTOR && PUBLIC VARIABLES                                          *
    //*******************************************************************************************************************

    //Constructor
    ABaseWeapon();

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Components")
    class USkeletalMeshComponent* WeaponMesh;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Components")
    class UAnimSequence* OnFireAnimation;

    UPROPERTY(EditDefaultsOnly, Category = "Misc")
    TSubclassOf<class APickupWeapon> SpawnWeaponOnDestroy;

    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ShootData")
    float ZoomFOV;

    //*******************************************************************************************************************
    //                                          PUBLIC FUNCTIONS                                                        *
    //*******************************************************************************************************************

    /*Spawn projectiles and increase heat value.*/
    virtual void FireWeapon();
    /*Stops firing weapon.*/
    virtual void StopFireWeapon();
    /*Used by weapon child to spawn a pickup weapon.*/
    virtual void DestroyThisWeapon();
    /*Increase bullet speed on slow time.*/
    virtual void SetFasterBulletOnSlowTime(const bool& bOnSlowTime, const float& NewTimeDilation);

    /*Called from player to increase cool down value and spawn smoke particles.*/
    void ForceCoolDownWeapon();
    /*Returns call down value back to default.*/
    void StopForceCoolDownWeapon();

    /*Returns the current heat value.*/
    float ReturnHeatValue();

    //*******************************************************************************************************************
    //                                          PUBLIC INLINE FUNCTIONS                                                 *
    //*******************************************************************************************************************

    FORCEINLINE bool WeaponIsOverHeatedValue() { return bWeaponIsOverHeated; }

protected:

    //*******************************************************************************************************************
    //                                          PROTECTED VARIABLES                                                     *
    //*******************************************************************************************************************

    class UCameraComponent* PlayerCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    class UParticleSystemComponent* CooldownWeaponSmoke;
    UPROPERTY(EditDefaultsOnly, Category = "SFX")
    class USoundCue* OverHeatShotSound;

    FTimerHandle CoolDown_TimerHandle;
    FTimerHandle EnableCoolDown_TimerHandle;

    bool bIsPlayerWeapon;
    bool bWeaponIsOverHeated;
    bool bIsShooting;
    bool bStopCoolDown;
    bool bPlayerOnSlowTime;

    const float MaxOverHeatAmount = 100.0f;
    const float CoolDownRate      = 0.1f;

    float InitialCoolDownAmount;
    float ActualWeaponHeat;
    float WeaponFasterTimeDilation;

    /*Overheat weapon amount*/
    UPROPERTY(EditDefaultsOnly, Category = "CoolDown Data")
    float OverheatAmount;
    /*Cool down weapon amount*/
    UPROPERTY(EditDefaultsOnly, Category = "CoolDown Data")
    float CoolDownAmount;
    /*Multiply cool down for this value when player press CoolDownWeapon.*/
    UPROPERTY(EditDefaultsOnly, Category = "CoolDown Data")
    float PowerCoolDown;
    /*Time to cool down again after shoot.*/
    UPROPERTY(EditDefaultsOnly, Category = "CoolDown Data")
    float EnableCoolDownTimer;
    UPROPERTY(EditDefaultsOnly, Category = "ShootData")
    float ShootLength;
    UPROPERTY(EditDefaultsOnly, Category = "ShootData")
    float MaxDamage;
    UPROPERTY(EditDefaultsOnly, Category = "ShootData")
    float MinDamage;
    UPROPERTY(EditDefaultsOnly, Category = "ShootData")
    float CriticChance;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<class ABaseProjectile> ProjectileClass;

    //*******************************************************************************************************************
    //                                          PROTECTED FUNCTIONS                                                     *
    //*******************************************************************************************************************

    /*Stops cool down.*/
    virtual void OverHeatWeapon();
    /*Used so all the weapon's child can execute the same BeginPlay.*/
    virtual void WeaponBeginPlay();

    /*Calls CoolDownWeapon every X seconds.*/
    void StartCoolDownWeapon();
    /*Reduces weapon heat.*/
    void CoolDownWeapon();
    /*Checks if weapon is overheated.*/
    void CheckCurrentHeat();
    /*Enable cool down after overheat.*/
    void EnableCoolDown();

    /*Returns where to shoot from the WeaponMuzzle to the Center of the camera.*/
    FTransform CalculateShootInfo();

    /*Saves the player camera reference.*/
    UFUNCTION(BlueprintCallable)
    void SetCamera(class UCameraComponent* Camera);
};