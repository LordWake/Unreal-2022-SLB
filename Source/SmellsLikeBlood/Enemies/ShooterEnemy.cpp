//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#include "ShooterEnemy.h"

#include "Components/ArrowComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Engine/DecalActor.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#include "SmellsLikeBlood/CommonObjects/BaseCoverObject.h"
#include "SmellsLikeBlood/CommonObjects/EnemyManager.h"
#include "SmellsLikeBlood/CommonObjects/PickupWeapon.h"
#include "SmellsLikeBlood/Player/TPS_PlayerCharacter.h"
#include "SmellsLikeBlood/Projectiles/BaseProjectile.h"

#include "Sound/SoundCue.h"

static FName AIM_TO_SPINE_NAME("spine_03");
static FName DEAD_COLLISION_SETTINGS("DeadEnemy");

//------------------------------------------------------------------------------------------------------------------------------------------
AShooterEnemy::AShooterEnemy()
{
    PrimaryActorTick.bCanEverTick = true;

    GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));
    GetMesh()->SetReceivesDecals(false);

    WeaponMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMeshComp"));
    WeaponMeshComp->SetupAttachment(GetMesh());

    Waypoint1BillboardComp = CreateDefaultSubobject<UBillboardComponent>(TEXT("Waypoint1BillboardComp"));
    Waypoint2BillboardComp = CreateDefaultSubobject<UBillboardComponent>(TEXT("Waypoint2BillboardComp"));

    Waypoint1BillboardComp->SetupAttachment(GetMesh());
    Waypoint2BillboardComp->SetupAttachment(GetMesh());
    Waypoint1BillboardComp->SetVisibility(false);
    Waypoint2BillboardComp->SetVisibility(false);

    MuzzleArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("MuzzleArrowComp"));
    MuzzleArrowComp->SetupAttachment(WeaponMeshComp);

    SpawnFloorBloodArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Blood Arrow"));
    SpawnFloorBloodArrow->SetupAttachment(GetMesh());

    bUseControllerRotationYaw = false;

    RagdollThrowForce         = 8000.0f;
    AlertEnemiesRadius        = 1000.0f;
    MaxDistanceToCover        = 3000.0f;
    MinDistanceAwayFromPlayer = 500.0f;
    HeadBoneName              = "head";
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AShooterEnemy::BeginPlay()
{
    Super::BeginPlay();

    DefaultRagdollThrowForce = RagdollThrowForce;
    CurrentHealth            = MaxHealth;
    LastBoneHit              = HeadBoneName;

    Waypoint_1 = Waypoint1BillboardComp->GetComponentLocation();
    Waypoint_2 = Waypoint2BillboardComp->GetComponentLocation();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AShooterEnemy::CalculateHealth(const float& MinDamage, const float& MaxDamage, const float& CriticChance, const FName BoneName)
{
    if (!bIsDead)
    {
        float LocalDamage = 0.0f;

        LocalDamage = static_cast<float>(UKismetMathLibrary::RandomFloatInRange(MinDamage, MaxDamage));
        if (UKismetMathLibrary::RandomFloatInRange(0.0f, 100.0f) <= CriticChance)
        {
            LocalDamage = LocalDamage * 2.0f;
        }

        if (BoneName == HeadBoneName)
        {
            LocalDamage = LocalDamage * 100.0f;
        }

        CurrentHealth = CurrentHealth - LocalDamage;
        if (CurrentHealth <= 0.0f)
        {
            CurrentHealth = 0.0f;
            bIsDead = true;
        }
    }
}

#pragma region Shoot Voids

//------------------------------------------------------------------------------------------------------------------------------------------
void AShooterEnemy::FireSniperRifle()
{
    if (!bIsFiring)
    {
        bIsAiming = true;
        bIsFiring = true;
        
        #pragma region DELAY
        FLatentActionInfo LatentInfo;
        LatentInfo.CallbackTarget    = this;
        LatentInfo.ExecutionFunction = FName("FireSniperRifle_PostDelay");
        LatentInfo.Linkage           = 0;
        LatentInfo.UUID              = GiveMeAnUUINumber();

        float SniperRifleDelay = FMath::FRandRange(2.0f, 3.0f);

        UKismetSystemLibrary::Delay(GetWorld(), SniperRifleDelay, LatentInfo);
        #pragma endregion
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AShooterEnemy::FireSniperRifle_PostDelay()
{
    if (bIsFiring)
    {
        WeaponMeshComp->PlayAnimation(OnFireSniperRifleAnimation, false);
        FTransform ProjectileSpawnInfo = CalculateProjectileInfo(SniperRifleSpread);

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        SpawnParams.bNoFail                        = true;
        SpawnParams.Instigator                     = this->GetInstigator();

        ABaseProjectile* SpawnedProjectile = GetWorld()->SpawnActor<ABaseProjectile>(SniperRifleProjectile, ProjectileSpawnInfo, SpawnParams);
        SpawnedProjectile->SetDamageInfo(EnemyMinDamage * 2.0f, EnemyMaxDamage * 2.0f, 0.0f);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AShooterEnemy::FireRifle()
{
    if (!bIsFiring)
    {
        bIsAiming = true;
        bIsFiring = true;

        #pragma region DELAY
        FLatentActionInfo LatentInfo;
        LatentInfo.CallbackTarget    = this;
        LatentInfo.ExecutionFunction = FName("FireRifle_PostDelay");
        LatentInfo.Linkage           = 0;
        LatentInfo.UUID              = GiveMeAnUUINumber();

        UKismetSystemLibrary::Delay(GetWorld(), RifleShotDelay, LatentInfo);
        #pragma endregion
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AShooterEnemy::FireRifle_PostDelay()
{
    if (bIsFiring)
    {
        const int32 MinRifleBurst = 5;
        const int32 MaxRifleBurst = 10;

        RifleBurstCount = FMath::RandRange(MinRifleBurst, MaxRifleBurst);
        RifleBurst();
        GetWorldTimerManager().SetTimer(TimerHandle, this, &AShooterEnemy::RifleBurst, RifleBurstTimerRate, true, 0.0f);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AShooterEnemy::RifleBurst()
{
    if (FireCount < RifleBurstCount)
    {
        WeaponMeshComp->PlayAnimation(OnFireRifleAnimation, false);

        FTransform ProjectileSpawnInfo = CalculateProjectileInfo(RifleSpread);

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        SpawnParams.bNoFail                        = true;
        SpawnParams.Instigator                     = this->GetInstigator();

        ABaseProjectile* SpawnedProjectile = GetWorld()->SpawnActor<ABaseProjectile>(RifleProjectile, ProjectileSpawnInfo, SpawnParams);
        SpawnedProjectile->SetDamageInfo(EnemyMinDamage, EnemyMaxDamage, 0.0f);

        ++FireCount;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AShooterEnemy::FireShotGun()
{
    if (!bIsFiring)
    {
        bIsFiring = true;

        #pragma region DELAY
        FLatentActionInfo LatentInfo;
        LatentInfo.CallbackTarget    = this;
        LatentInfo.ExecutionFunction = FName("FireShotGun_PostDelay");
        LatentInfo.Linkage           = 0;
        LatentInfo.UUID              = GiveMeAnUUINumber();

        UKismetSystemLibrary::Delay(GetWorld(), ShotGunDelay, LatentInfo);
        #pragma endregion
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AShooterEnemy::FireShotGun_PostDelay()
{
    if (bIsFiring)
    {
        bIsAiming = true;
        ShotGunBurst();
        GetWorldTimerManager().SetTimer(TimerHandle, this, &AShooterEnemy::ShotGunBurst, ShotGunTimerRate, true, 0.0f);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AShooterEnemy::ShotGunBurst()
{
    uint8_t FireCountLimit = 3;
    if (FireCount < FireCountLimit)
    {
        WeaponMeshComp->PlayAnimation(OnFireShotGunAnimation, false);

        for (int32 i = 1; i < ShotgunBurstCount; ++i)
        {
            FTransform ProjectileSpawnInfo = CalculateProjectileInfo(ShotgunSpread);

            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
            SpawnParams.bNoFail                        = true;
            SpawnParams.Instigator                     = this->GetInstigator();

            ABaseProjectile* SpawnedProjectile = GetWorld()->SpawnActor<ABaseProjectile>(ShotGunProjectile, ProjectileSpawnInfo, SpawnParams);

            const float MinDamageDivided = static_cast<float>(EnemyMinDamage / ShotgunBurstCount);
            const float MaxDamageDivided = static_cast<float>(EnemyMaxDamage / ShotgunBurstCount);

            SpawnedProjectile->SetDamageInfo(MinDamageDivided, MaxDamageDivided, 0.0f);
        }

        ++FireCount;
    }
    else
    {
        #pragma region DELAY
        FLatentActionInfo LatentInfo;
        LatentInfo.CallbackTarget    = this;
        LatentInfo.ExecutionFunction = FName("ShotGunBurst_PostDelay");
        LatentInfo.Linkage           = 0;
        LatentInfo.UUID              = GiveMeAnUUINumber();

        UKismetSystemLibrary::Delay(GetWorld(), ShotGunBurstDelay, LatentInfo);
        #pragma endregion
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AShooterEnemy::ShotGunBurst_PostDelay()
{
    FireCount = 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AShooterEnemy::StopFireWeapon()
{
    bIsFiring = false;
    bIsAiming = false;
    FireCount = 0;

    if (TimerHandle.IsValid())
    {
        GetWorldTimerManager().ClearTimer(TimerHandle);
    }
}

#pragma endregion

//------------------------------------------------------------------------------------------------------------------------------------------
void AShooterEnemy::EnemyMeleeAttack()
{
    SetIsTakingCover(false);
    bOnMeleeAttack = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AShooterEnemy::EnemyMeleeHit()
{
    const FRotator& LookToPlayer_Rot = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(),
                                       PlayerCharacter->GetActorLocation());

    //Look direct to player.
    SetActorRotation(FRotator(GetActorRotation().Pitch, LookToPlayer_Rot.Yaw, GetActorRotation().Roll));

    FHitResult HitResult;

    const FVector& StartTrace = GetActorLocation();
    const FVector& EndTrace   = StartTrace + (GetMesh()->GetRightVector() * MeleeDistance);

    TArray<AActor*> IgnoreThis;
    IgnoreThis.Add(this);

    //Make a BoxTrace to check if the player is in front of us.
    const bool BoxTrace = UKismetSystemLibrary::BoxTraceSingle(GetWorld(), StartTrace, EndTrace,
                          FVector(1.0f, 30.0f, 30.0f), FRotator(0.0f, 0.0f, 0.0f), static_cast<ETraceTypeQuery>(WEAPON_TRACE),
                          true, IgnoreThis, EDrawDebugTrace::None, HitResult, true);

    if (BoxTrace)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), CombatAudio, GetActorLocation());
        if (UKismetSystemLibrary::DoesImplementInterface(HitResult.GetActor(), UDamageInterface::StaticClass()))
        {
            IDamageInterface::Execute_TakeDamage(HitResult.GetActor(), MinMeleeDamage, MaxMeleeDamage, 0.0f, HitResult.BoneName, this->GetInstigator());
            UGameplayStatics::PlaySoundAtLocation(GetWorld(), MeleeHit, GetActorLocation());
            UGameplayStatics::PlayWorldCameraShake(GetWorld(), MeleeCameraShake, GetActorLocation(), 0.0f, 500.0f, 1.0f, false);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AShooterEnemy::SetIsTakingCover(const bool bEnemyIsTakingCover)
{
    bIsTakingCover = bEnemyIsTakingCover;
    if (LastCoverReference != nullptr)
    {
        LastCoverReference->SetCoverIsUsed(bIsTakingCover);
    }

    if (bIsTakingCover)
    {
        SetActorRotation(_CoverRotation);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AShooterEnemy::PlayCombatAudio()
{
    if (bEnemyIsInCombatStatus && !bIsDead && !bIsACorpse)
    { 
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), CombatAudio, GetActorLocation()); 
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AShooterEnemy::TakeDamage_Implementation(float DamageMin, float DamageMax, float CriticChance, FName HitBoneName, AActor* DamageInstigator)
{
    if (Cast<AShooterEnemy>(DamageInstigator))
    {
        return;
    }

    LastBoneHit = HitBoneName;
    DamageVFX(HitBoneName);

    if (!bIsACorpse)
    {
        ChangeCombatStatus(DamageInstigator);
        CalculateHealth(DamageMin, DamageMax, CriticChance, HitBoneName);
    }

    if (bIsDead)
    {
        StopFireWeapon();
        OnEnemyDead();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AShooterEnemy::IsCoverAvailable_Implementation(bool bIsCoverAvailable, FRotator CoverRotation)
{
    bCoverAvailable = bIsCoverAvailable;
    _CoverRotation  = CoverRotation;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AShooterEnemy::EndOfCover_Implementation(bool bEndOfCoverLeft, bool bEndOfCoverRight)
{
            //NOT USED FROM ENEMY.
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AShooterEnemy::DamageVFX(const FName& BoneName)
{
    const int32& SelectIndex = UKismetMathLibrary::RandomIntegerInRange(0, BloodFloor.Num() - 1);

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    SpawnParams.bNoFail                        = true;

    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodVFX, GetMesh()->GetBoneLocation(BoneName), FRotator::ZeroRotator, FVector(1.f), true, EPSCPoolMethod::AutoRelease, true);
    GetWorld()->SpawnActor<ADecalActor>(BloodFloor[SelectIndex], SpawnFloorBloodArrow->GetComponentTransform(), SpawnParams);
}

//------------------------------------------------------------------------------------------------------------------------------------------
FTransform AShooterEnemy::CalculateProjectileInfo(const float& Spread)
{
    const FVector  InitialLocation = MuzzleArrowComp->GetComponentLocation();
    const FRotator LookAtRotation  = UKismetMathLibrary::FindLookAtRotation(InitialLocation, PlayerCharacter->GetMesh()->GetSocketLocation(AIM_TO_SPINE_NAME));
    const FVector  RandomUnit      = UKismetMathLibrary::RandomUnitVectorInConeInRadians(UKismetMathLibrary::GetForwardVector(LookAtRotation), Spread);

    FTransform ReturnTransform = FTransform(UKismetMathLibrary::Conv_VectorToRotator(RandomUnit), InitialLocation, FVector(1.0f, 1.0f, 1.0f));
    return ReturnTransform;
}

//------------------------------------------------------------------------------------------------------------------------------------------
int32 AShooterEnemy::GiveMeAnUUINumber()
{
    if (currentUUIDValue > maxUUIDValue)
    {
        currentUUIDValue = 0;
        return currentUUIDValue;
    }
    else
    {
        return ++currentUUIDValue;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AShooterEnemy::OnEnemyDead()
{
    //If is not a corpse, it is dying. So, we add slow time points to the player and spawn a pickup weapon.
    if (!bIsACorpse)
    {
        if (LastCoverReference)
        {
            LastCoverReference->SetCoverIsUsed(false);
        }

        WeaponMeshComp->DestroyComponent(false);
        PlayerCharacter->EnemyKilledByPlayer();

        FActorSpawnParameters ActorSpawnParams;
        ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        GetWorld()->SpawnActor<APickupWeapon>(OnDeadWeaponSpawn, WeaponMeshComp->GetComponentTransform(), ActorSpawnParams);

        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        GetMesh()->SetSimulatePhysics(true);
        GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
        GetMesh()->SetCollisionProfileName(DEAD_COLLISION_SETTINGS);

        GetWorldTimerManager().SetTimer(PostDead_TimerHandle, this, &AShooterEnemy::ActivateBloodOnFloor, 1.0f, false);
        if (MyEnemyManager)
        {
            MyEnemyManager->OnEnemyKilled(this);
        }
    }
    else
    {
        RagdollThrowForce = DefaultRagdollThrowForce / 4;
    }

    const FVector& DeathImpulse = UKismetMathLibrary::Normal(PlayerCharacter->GetActorForwardVector());
    GetMesh()->AddImpulse(DeathImpulse * RagdollThrowForce, LastBoneHit, true);

    bIsACorpse = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AShooterEnemy::ActivateBloodOnFloor()
{
    GetMesh()->SetReceivesDecals(true);
}