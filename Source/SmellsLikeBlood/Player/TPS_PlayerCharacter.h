//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"

#include "SmellsLikeBlood/SmellsLikeBlood.h"
#include "SmellsLikeBlood/Interfaces/DamageInterface.h"
#include "SmellsLikeBlood/Interfaces/CoverInterface.h"

#include "TPS_PlayerCharacter.generated.h"

UENUM(BlueprintType)
enum class ECurrentWeapon : uint8
{
    AssaultRifleWeapon,
    LegendaryAssaultRifle,
    ShotgunWeapon,
    LegendaryShotgun,
    SniperRifleWeapon,
    LegendarySniperWeapon
};

UCLASS()
class SMELLSLIKEBLOOD_API ATPS_PlayerCharacter : public ACharacter, public IDamageInterface, public ICoverInterface
{
    GENERATED_BODY()

    friend class ATPS_PlayerController;

public:

    //*******************************************************************************************************************
    //                                         CONSTRUCTOR && PUBLIC COMPONENTS                                         *
    //*******************************************************************************************************************

    //Constructor
    ATPS_PlayerCharacter();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* SpringArmComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    class UArrowComponent* SpawnFloorBloodArrow;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
    class UCameraComponent* CameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Effects", meta = (AllowPrivateAccess = "true"))
    class UPostProcessComponent* SlowTimePostProcess;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Effects", meta = (AllowPrivateAccess = "true"))
    class UPostProcessComponent* LowHealthPostProcess;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
    class UPawnNoiseEmitterComponent* NoiseEmitter;

    UPROPERTY(BlueprintReadOnly, Category = "Weapons")
    class ABaseWeapon* EquippedWeapon;

    UPROPERTY(EditDefaultsOnly, Category = "VFX")
    TSubclassOf<class UMatineeCameraShake> MeleeCameraShake;

    //*******************************************************************************************************************
    //                                         PUBLIC VARIABLES                                                         *
    //*******************************************************************************************************************

    class ATPS_PlayerController* PlayerController;

    /*Weapon Slots, called from player controller to handle UI, and from weapon pickup.*/
    UPROPERTY(BlueprintReadOnly, Category = "Weapons")
    class AAssaultRifle* AssaultRifleSlot;
    UPROPERTY(BlueprintReadOnly, Category = "Weapons")
    class AShotgun* ShotgunSlot;
    UPROPERTY(BlueprintReadOnly, Category = "Weapons")
    class ASniperRifleWeapon* SniperRifleSlot;

    float CurrentSlowTimer;

    bool bPlayerCanFire;
    bool bCoverAvailable;
    bool bIsSlowTimeActivated;

    UPROPERTY(BlueprintReadWrite, Category = "Animation Variables")
    bool bPlayerIsTakingCover;
    UPROPERTY(BlueprintReadWrite, Category = "Animation Variables")
    bool bPlayerEndOfCoverLeft;
    UPROPERTY(BlueprintReadWrite, Category = "Animation Variables")
    bool bPlayerEndOfCoverRight;
    UPROPERTY(BlueprintReadWrite, Category = "Animation Variables")
    bool bPlayerOnMeleeAttack;
    UPROPERTY(BlueprintReadOnly, Category = "Player's Death")
    bool bIsDead;

    FRotator _CoverRotation;

    /* Used in the AnimBP. */
    UPROPERTY(BlueprintReadOnly, Category = "Weapons")
    ECurrentWeapon CurrentWeapon;

    //*******************************************************************************************************************
    //                                          PUBLIC FUNCTIONS                                                        *
    //*******************************************************************************************************************

    bool TakeNewWeapon(EAllWeaponTypes WeaponType);

    void EnemyKilledByPlayer();
    void BulletSpawnedFeedBack();

    //*******************************************************************************************************************
    //                                          PUBLIC INLINE FUNCTIONS                                                 *
    //*******************************************************************************************************************

    FORCEINLINE void SetLastCoverReference(class ABaseCoverObject* LastRef) { LastCoverReference = LastRef; }

    //*******************************************************************************************************************
    //                                          INTERFACES FUNCTIONS                                                    *
    //*******************************************************************************************************************

    virtual void TakeDamage_Implementation(float DamageMin, float DamageMax, float CriticChance, FName HitBoneName, AActor* DamageInstigator) override;
    virtual void IsCoverAvailable_Implementation(bool bIsCoverAvailable, FRotator CoverRotation) override;
    virtual void EndOfCover_Implementation(bool bEndOfCoverLeft, bool bEndOfCoverRight) override;

protected:

    //*******************************************************************************************************************
    //                                          PROTECTED VARIABLES                                                     *
    //*******************************************************************************************************************

    UPROPERTY(BlueprintReadOnly)
    float CurrentFOVValue;
    UPROPERTY(BlueprintReadOnly)
    float DefaultFOVValue;
    UPROPERTY(BlueprintReadOnly)
    float CoverFOVValue;

    //*******************************************************************************************************************
    //                                          PROTECTED FUNCTIONS                                                     *
    //*******************************************************************************************************************

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    /*Checks if we have taken weapons from past level and load it in this one.*/
    void LoadPlayerWeaponsFromSaveFile();

    /*Master function to spawn weapons, choose a weapon type, where to attach it, if it will be equipped
    and if we already have one weapon in this slow we should throw it and spawn a pickupWeapon.*/
    void SpawnAndAttachWeapon(EAllWeaponTypes WeaponType, const FName& AttachToThisSocket, const bool& bEquipThisOne, const bool& bThrowWeapon);
    /*Calls SetTakingCover*/
    void TakeCover();
    /*Tells the equipped weapon to start fire.*/
    void FireWeapon();
    /*Tells the equipped weapon to stop fire.*/
    void StopFireWeapon();
    /*Executes animation.*/
    void PlayerStartsMeleeAttack();

    /*Called from PlayerController to switch between the weapons slots.*/
    void SwitchWeapons(EAllWeaponTypes SwitchToThis);
    /*Maximize cool down value when pressing "X".*/
    void ForceWeaponCoolDown(const bool& bForceWeaponCoolDown);

    /*Checks cover angle so in case we press the cover button and the cover
    is behind us, it won't take cover because it makes no sense.*/
    bool CheckCoverAngle();

    /*Called from the AnimBlueprint in AnimNotify.*/
    UFUNCTION(BlueprintCallable)
    void PlayerEndsMeleeAttack();

    /*Sets bPlayerIsTakingCover value so the AnimBP can do its magic.*/
    UFUNCTION(BlueprintCallable)
    void SetIsTakingCover(const bool bIsTakingCover);

    /*Called from the AnimBlueprint so we can't fire in certain animations. Like Sprinting.*/
    UFUNCTION(BlueprintCallable)
    void SetCanFire(bool bCanFire);

    /*Called from the AnimBlueprint in AnimNotify.*/
    UFUNCTION(BlueprintCallable)
    void OnMeleeHit();

    /*Makes zoom effect using blueprints and time lines.*/
    UFUNCTION(BlueprintImplementableEvent)
    void SetFOV(bool bIsAiming);

    /*Makes zoom effect using blueprints and time lines. Called from AnimBP.*/
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void SetCoverFOV(bool bOnCover);

    /*Makes zoom out when sprinting.*/
    UFUNCTION(BlueprintImplementableEvent)
    void PlayerIsSprinting(bool bPlayerIsSprinting);

private:

    //*******************************************************************************************************************
    //                                          PRIVATE VARIABLES                                                       *
    //*******************************************************************************************************************

    class USLB_GameInstance* GameInstanceReference;
    class ABaseCoverObject* LastCoverReference;

    /*ASSAULT RIFLES (Normal && Legendary)*/
    UPROPERTY(EditDefaultsOnly, Category = "Weapons")
    TSubclassOf<class AAssaultRifle> AssaultRifle;
    UPROPERTY(EditDefaultsOnly, Category = "Weapons")
    TSubclassOf<class AAssaultRifle> LegendAssaultRifle;

    /*SHOTGUNS (Normal && Legendary)*/
    UPROPERTY(EditDefaultsOnly, Category = "Weapons")
    TSubclassOf<class AShotgun> Shotgun;
    UPROPERTY(EditDefaultsOnly, Category = "Weapons")
    TSubclassOf<class AShotgun> LegendShotgun;

    /*SNIPER RIFLES (Normal && Legendary)*/
    UPROPERTY(EditDefaultsOnly, Category = "Weapons")
    TSubclassOf<class ASniperRifleWeapon> Sniper;
    UPROPERTY(EditDefaultsOnly, Category = "Weapons")
    TSubclassOf<class ASniperRifleWeapon> LegendSniper;

    UPROPERTY(EditDefaultsOnly, Category = "VFX")
    UParticleSystem* BloodVFX;

    UPROPERTY(EditDefaultsOnly, Category = "VFX")
    TArray<TSubclassOf<class ADecalActor>> BloodFloor;

    UPROPERTY(EditDefaultsOnly, Category = "SFX")
    class USoundCue* SlowTimeActivated;
    UPROPERTY(EditDefaultsOnly, Category = "SFX")
    class USoundCue* MeleeHit;
    UPROPERTY(EditDefaultsOnly, Category = "SFX")
    class USoundCue* TakeWeaponSound;
    UPROPERTY(EditDefaultsOnly, Category = "SFX")
    class USoundCue* KilledEnemySound;
    UPROPERTY(EditDefaultsOnly, Category = "SFX")
    class USoundCue* TakeDamageSound;
    UPROPERTY(EditDefaultsOnly, Category = "SFX")
    class USoundCue* Sound_HeartBeat;
    UPROPERTY(EditDefaultsOnly, Category = "SFX")
    class USoundCue* SwitchToRifle;
    UPROPERTY(EditDefaultsOnly, Category = "SFX")
    class USoundCue* SwitchToShotgun;
    UPROPERTY(EditDefaultsOnly, Category = "SFX")
    class USoundCue* SwitchToSniper;

    const float MaxHealth           = 100.0f;
    const float EndDamageTimerRate  = 2.5f;
    const float CoverDamageModifier = 0.8f;
    const float MaxSlowTimer        = 100.0f;
    const float MeleeDistance       = 150.0f;
    const float PP_SlowTime_Speed   = 3.0f;

    /*Attack Data*/
    UPROPERTY(EditDefaultsOnly, Category = "Melee")
    float MinMeleeDamage;
    UPROPERTY(EditDefaultsOnly, Category = "Melee")
    float MaxMeleeDamage;
    UPROPERTY(EditDefaultsOnly, Category = "Melee")
    float CriticChanceMeleeDamage;

    /*Slow Time Data*/
    UPROPERTY(EditDefaultsOnly, Category = "Slow Timer")
    float SlowWorldTimeDilation;
    UPROPERTY(EditDefaultsOnly, Category = "Slow Timer")
    float PlayerCustomTimeDilation;
    UPROPERTY(EditDefaultsOnly, Category = "Slow Timer")
    float SlowTimerAddValue;
    UPROPERTY(EditDefaultsOnly, Category = "Slow Timer")
    float SlowTimerConsumeValue;

    float CurrentHealth;
    float HealthRegenerationValue;

    bool bIsTakingDamage;
    bool bPlayerCanSwitchWeapons;
    bool bIsSwitchingWeapons;

    uint8_t CurrentEnemiesKilled;
    uint8_t EnemiesGoalToSaySomething;

    FTimerHandle EnemyTarget_TimerHandle;
    FTimerHandle Damage_TimerHandle;
    FTimerHandle RegenerateHealth_TimerHandle;
    FTimerHandle WeaponHeat_TimerHandle;
    FTimerHandle SlowTimer_TimeHandle;
    FTimerHandle LowHealthFB_TimeHandle;

    //*******************************************************************************************************************
    //                                          PRIVATE FUNCTIONS                                                       *
    //*******************************************************************************************************************

    /*Calls AddHealth function to recover life every X seconds.*/
    void StartRegenerateHealth();
    /*Calls PerformUpdateHeatValue to refresh heat value on UI.*/
    void StartUpdateWeaponHeat();
    /*Calls ConsumeSlowTimeBullet to refresh Slow time value on UI (if it's activated).*/
    void StartUpdateSlowTimer();
    /*Calls PerformUpdateEnemyOnTarget to check if we are aiming to an enemy an update hit mark UI.*/
    void StartCheckEnemiesOnTarget();
    /*Calls LowHealthRumbleFeedBack to check if life is below X% of its value.*/
    void StartCheckLowHealthFeedBack();
    /*Takes a random value between Min and Max floats and applies damage.*/
    void CalculateHealth(const float& MinHealthDamage, const float& MaxHealthDamage);
    /*Updates health value to the player controller.*/
    void PerformUpdateHealth();
    /*Interrupt or allow the possibility of shooting or taking weapons.*/
    void PlayerCanFireAndTakeWeapons(const bool& bPlayerCan);
    /*Enables SlowTime, executes the mechanic, plays a sound and sets Post Processing on camera.*/
    void PlayerSlowTimeBullet();
    /*Returns Time Dilation back to normal. Disables slow time.*/
    void PlayerStopSlowTimeBullet();
    /*Updates the current weapon equipped to the player controller.*/
    void SetNewWeaponHUD();
    /*Updates what type of weapon we have (normal or legendary) to the player controller.*/
    void UpdateWeaponSlotsHUD();
    /*Tells the playerController to show the weapon UI on switch weapons.*/
    void UpdateSelectedWeaponHUD();
    /*Checks if it has to set any of the two Post process.*/
    void CheckPostProcessEffects(float DeltaTimer);
    /*Activates the low health post process and the heartbeat sound.*/
    void HandleLowHealthEffects(float DeltaTimerRef);
    /*Activates slow time post process.*/
    void HandleSlowTimePP(float DeltaTimerRef);
    /*Spawns blood particles and a decal when the player takes damage.*/
    void DamageVFX(const FName& BoneName);
    /*Tells Player Controller that this player is dead, handles ragdoll, etc.*/
    void OnPlayersDeath();

    /*Allows the player to recover health again.*/
    UFUNCTION()
    void EndDamageTimer();
    /*Called from StartRegenerateHealth. Adds health.*/
    UFUNCTION()
    void AddHealth();
    /*Updates heat value and, if the weapon is overheated, to the player controller.*/
    UFUNCTION()
    void PerformUpdateHeatValue();
    /*Updates hit mark state to the player controller so it can switch between white or red.*/
    UFUNCTION()
    void PerformUpdateEnemyOnTarget();
    /*If slow time is activated, it will consume his value and update it to the player controller.*/
    UFUNCTION()
    void ConsumeSlowTimeBullet();
    /*If player health is below X% it activates joystick rumble with the heartbeat.*/
    UFUNCTION()
    void LowHealthRumbleFeedBack();
};