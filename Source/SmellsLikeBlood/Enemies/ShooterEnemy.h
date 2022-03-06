//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "SmellsLikeBlood/Interfaces/CoverInterface.h"
#include "SmellsLikeBlood/Interfaces/DamageInterface.h"

#include "ShooterEnemy.generated.h"

UCLASS()
class SMELLSLIKEBLOOD_API AShooterEnemy : public ACharacter, public IDamageInterface, public ICoverInterface
{
    GENERATED_BODY()

public:

    //*******************************************************************************************************************
    //                                         CONSTRUCTOR && PUBLIC COMPONENTS                                         *
    //*******************************************************************************************************************

    //Constructor
    AShooterEnemy();

    UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = "Player Reference")
    class ATPS_PlayerCharacter* PlayerCharacter;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
    class USkeletalMeshComponent* WeaponMeshComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
    class UBillboardComponent* Waypoint1BillboardComp;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
    class UBillboardComponent* Waypoint2BillboardComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    class UArrowComponent* MuzzleArrowComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    class UArrowComponent* SpawnFloorBloodArrow;

    //*******************************************************************************************************************
    //                                         PUBLIC VARIABLES                                                         *
    //*******************************************************************************************************************

    UPROPERTY(BlueprintReadWrite)
    bool bEnemyIsInCombatStatus;
    /*Set this to true if the AI is not being spawned by an Enemy manager*/
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Data")
    bool bAIIsEnable;

    /*Used from BTTask_FindCover*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Data")
    float MaxDistanceToCover;
    /*Used from BTTask_FindCover*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Data")
    float MinDistanceAwayFromPlayer;

    /*Called from BTTask_GetWaypoints*/
    UPROPERTY(BlueprintReadOnly, Category = "Waypoints")
    FVector Waypoint_1;
    /*Called from BTTask_GetWaypoints*/
    UPROPERTY(BlueprintReadOnly, Category = "Waypoints")
    FVector Waypoint_2;

    //*******************************************************************************************************************
    //                                          PUBLIC FUNCTIONS                                                        *
    //*******************************************************************************************************************

    /*Called from BTTask_StartStopFiringWeapon.*/
    UFUNCTION(BlueprintCallable)
    void StopFireWeapon();

    /*Called from BTTask_StartStopFiringWeapon & used in BP_ShooterEnemy to switch the Unreal Enumeration.*/
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void FireWeapon();

    /*Called from BTTask_MeleeAttack to start attack animation.*/
    UFUNCTION(BlueprintCallable)
    void EnemyMeleeAttack();

    /*Called from AnimBP to make the actually physic attack.*/
    UFUNCTION(BlueprintCallable)
    void EnemyMeleeHit();

    /*Called from BTTask_SetIsTakingCover.*/
    UFUNCTION(BlueprintCallable)
    void SetIsTakingCover(const bool bEnemyIsTakingCover);

    /*Called from other enemies to make everyone near this enemy start combat status AI.*/
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void SetEnemyToCombatStatus(APawn* SensedPawnRef);

    /*Called from EnemyManager to play combat audio when enemy is in combat status.*/
    void PlayCombatAudio();

    //*******************************************************************************************************************
    //                                          PUBLIC INLINE FUNCTIONS                                                 *
    //*******************************************************************************************************************

    /*Called from BaseCoverObject.*/
    FORCEINLINE void SetLastCoverReference(class ABaseCoverObject* LastRef) { LastCoverReference = LastRef; }
    /*Called from Player to know if hit mark should be red or white. If this enemy is dead, 
    there is no reason to make hit mark look red.*/
    FORCEINLINE bool GetIfEnemyIsDead() { return bIsDead; }

    /*Called from EnemyManager to save a reference.*/
    FORCEINLINE void SetEnemyManager(class AEnemyManager* EnemyManagerRef) {MyEnemyManager = EnemyManagerRef;}

    /*Called from EnemyManager to automatic kill this enemy on end of level.*/
    FORCEINLINE const FName& HeadBoneNameReference() { return HeadBoneName; }

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

    UPROPERTY(BlueprintReadWrite)
    FTimerHandle TimerHandle;
    FTimerHandle PostDead_TimerHandle;

    class AEnemyManager* MyEnemyManager;

    UPROPERTY(EditDefaultsOnly, Category = "Components")
    class UAnimSequence* OnFireSniperRifleAnimation;
    UPROPERTY(EditDefaultsOnly, Category = "Components")
    class UAnimSequence* OnFireRifleAnimation;
    UPROPERTY(EditDefaultsOnly, Category = "Components")
    class UAnimSequence* OnFireShotGunAnimation;

    UPROPERTY(EditDefaultsOnly, Category   = "Components")
    TSubclassOf<class ABaseProjectile> SniperRifleProjectile;
    UPROPERTY(EditDefaultsOnly, Category   = "Components")
    TSubclassOf<class ABaseProjectile> RifleProjectile;
    UPROPERTY(EditDefaultsOnly, Category   = "Components")
    TSubclassOf<class ABaseProjectile> ShotGunProjectile;
    UPROPERTY(BlueprintReadWrite, Category = "Components")
    TSubclassOf<class APickupWeapon> OnDeadWeaponSpawn;

    UPROPERTY(EditDefaultsOnly, Category = "SFX")
    class USoundCue* MeleeHit;
    UPROPERTY(EditDefaultsOnly, Category = "SFX")
    class USoundCue* CombatAudio;

    UPROPERTY(EditDefaultsOnly, Category = "VFX")
    TSubclassOf<class UMatineeCameraShake> MeleeCameraShake;

    int32 FireCount;
    int32 RifleBurstCount;
    const int32 ShotgunBurstCount = 5;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Data")
    float AlertEnemiesRadius;

    float CurrentHealth;
    float DefaultRagdollThrowForce;

    const float MaxHealth         = 100.0f;
    const float ShotgunSpread     = 0.1f;
    const float RifleSpread       = 0.055f;
    const float SniperRifleSpread = 0.02f;
    const float EnemyMinDamage    = 4.0f;
    const float EnemyMaxDamage    = 8.0f;

    UPROPERTY(BlueprintReadWrite)
    FRotator _CoverRotation;

    bool bCoverAvailable;
    bool bIsFiring;
    bool bIsACorpse;

    /*Called from AnimBP*/
    UPROPERTY(BlueprintReadOnly)
    bool bIsAiming;
    /*Called from AnimBP*/
    UPROPERTY(BlueprintReadWrite)
    bool bOnMeleeAttack;
    /*Called from AnimBP*/
    UPROPERTY(BlueprintReadOnly)
    bool bIsTakingCover;
    /*Called from FireWeapon() in BP*/
    UPROPERTY(BlueprintReadOnly)
    bool bIsDead;

    //*******************************************************************************************************************
    //                                          PROTECTED FUNCTIONS                                                     *
    //*******************************************************************************************************************

    virtual void BeginPlay() override;
    
    /*Calculates how much damage this enemy takes depending on CriticChance and the bone name. Head bone makes a lot of damage.*/
    void CalculateHealth(const float& MinDamage, const float& MaxDamage, const float& CriticChance, const FName BoneName);

    /*Set in blueprints, called from TakeDamage_Implementation()*/
    UFUNCTION(BlueprintImplementableEvent)
    void ChangeCombatStatus(AActor* DamageInstigatorReference);

    //-------------------------------
    //SNIPER
    //-------------------------------
    UFUNCTION(BlueprintCallable)
    void FireSniperRifle();
    UFUNCTION()
    void FireSniperRifle_PostDelay();

    //-------------------------------
    //ASSAULT RIFLE
    //-------------------------------
    UFUNCTION(BlueprintCallable)
    void FireRifle();
    UFUNCTION()
    void FireRifle_PostDelay();
    UFUNCTION()
    void RifleBurst();

    //-------------------------------
    //SHOTGUN
    //-------------------------------
    UFUNCTION(BlueprintCallable)
    void FireShotGun();
    UFUNCTION()
    void FireShotGun_PostDelay();
    UFUNCTION()
    void ShotGunBurst();
    UFUNCTION()
    void ShotGunBurst_PostDelay();

private:

    //*******************************************************************************************************************
    //                                          PRIVATE VARIABLES                                                       *
    //*******************************************************************************************************************

    class ABaseCoverObject* LastCoverReference;

    UPROPERTY(EditDefaultsOnly, Category = "VFX")
    UParticleSystem* BloodVFX;

    UPROPERTY(EditDefaultsOnly, Category = "VFX")
    TArray<TSubclassOf<class ADecalActor>> BloodFloor;

    UPROPERTY(EditDefaultsOnly, Category = "Melee")
    float MinMeleeDamage;
    UPROPERTY(EditDefaultsOnly, Category = "Melee")
    float MaxMeleeDamage;
    UPROPERTY(EditDefaultsOnly, Category = "Damage")
    float RagdollThrowForce;

    const float MeleeDistance       = 150.0f;
    const float RifleShotDelay      = 1.5f;
    const float RifleBurstTimerRate = 0.233f;
    const float ShotGunDelay        = 1.25f;
    const float ShotGunTimerRate    = 1.0f;
    const float ShotGunBurstDelay   = 3.0f;

    const int32 maxUUIDValue = 25;
    int32 currentUUIDValue   = -1;

    UPROPERTY(EditDefaultsOnly, Category = "Damage")
    FName HeadBoneName;
    FName LastBoneHit;

    //*******************************************************************************************************************
    //                                          PRIVATE FUNCTIONS                                                       *
    //*******************************************************************************************************************

    /*Sets where the bullet is going to be spawned always aiming to player's spine bone.*/
    FTransform CalculateProjectileInfo(const float& Spread);

    int32 GiveMeAnUUINumber();

    /*Spawns random blood and decals on take damage.*/
    void DamageVFX(const FName& BoneName);

    /*Apply rag doll physics, spawns a pickupWeapon and set this enemy as a corpse.
    If it is a corpse, it will call this function just to apply some rag doll force and spawn more blood.*/
    void OnEnemyDead();

    /*Set the Receive Decals to enable in this Skeletal Mesh.*/
    UFUNCTION()
    void ActivateBloodOnFloor();
};