//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#include "TPS_PlayerCharacter.h"

#include "Camera/CameraComponent.h"

#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Components/PostProcessComponent.h"

#include "Engine/EngineTypes.h"
#include "Engine/DecalActor.h"

#include "GameFramework/SpringArmComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "Particles/ParticleSystemComponent.h"

#include "SmellsLikeBlood/CommonObjects/BaseCoverObject.h"
#include "SmellsLikeBlood/Enemies/ShooterEnemy.h"
#include "SmellsLikeBlood/GameFramework/SaveGameMaster.h"
#include "SmellsLikeBlood/GameFramework/SLB_GameInstance.h"
#include "SmellsLikeBlood/Player/TPS_PlayerController.h"
#include "SmellsLikeBlood/Weapons/AssaultRifle.h"
#include "SmellsLikeBlood/Weapons/BaseWeapon.h"
#include "SmellsLikeBlood/Weapons/Shotgun.h"
#include "SmellsLikeBlood/Weapons/SniperRifleWeapon.h"

#include "Sound/SoundCue.h"

#define SAVE_WEAPON_NORMAL    "Normal"
#define SAVE_WEAPON_LEGENDARY "Legendary"
#define LIFE_WARNING_PERCENT   30

static FName NAME_WeaponSocket("WeaponSocket");
static FName NAME_RifleBackSocket("RifleBackSocket");
static FName NAME_ShotgunBackSocket("ShotgunBackSocket");
static FName NAME_SniperBackSocket("SniperBackSocket");

//------------------------------------------------------------------------------------------------------------------------------------------
ATPS_PlayerCharacter::ATPS_PlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));

    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComponent->SetupAttachment(GetCapsuleComponent());
    SpringArmComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 60.0f));
    SpringArmComponent->TargetArmLength = 200.f;
    SpringArmComponent->SocketOffset = FVector(0.0f, 50.0f, 20.0f);
    SpringArmComponent->bUsePawnControlRotation = true;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComponent->SetupAttachment(SpringArmComponent);
    CameraComponent->bUsePawnControlRotation = true;

    SpawnFloorBloodArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Blood Arrow"));
    SpawnFloorBloodArrow->SetupAttachment(GetMesh());

    SlowTimePostProcess  = CreateDefaultSubobject<UPostProcessComponent>(TEXT("SlowTimePP"));
    LowHealthPostProcess = CreateDefaultSubobject<UPostProcessComponent>(TEXT("LowHealthPP"));
    SlowTimePostProcess->BlendWeight  = 0.0f;
    LowHealthPostProcess->BlendWeight = 0.0f;

    NoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("NoiseEmitter"));

    MinMeleeDamage           = 50.0f;
    MaxMeleeDamage           = 75.0f;
    CriticChanceMeleeDamage  = 50.0f;
    SlowWorldTimeDilation    = 0.25f;
    PlayerCustomTimeDilation = 5.0f;
    SlowTimerAddValue        = 20.0f;
    SlowTimerConsumeValue    = 5.0f;
    DefaultFOVValue          = 90.0f;
    CoverFOVValue            = 120.0f;
    CurrentFOVValue          = DefaultFOVValue;

    bPlayerCanFire = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    PlayerController      = Cast<ATPS_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    GameInstanceReference = CastChecked<USLB_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

    bPlayerCanSwitchWeapons = true;
    HealthRegenerationValue = 1.0f;
    CurrentHealth           = MaxHealth;
    CurrentSlowTimer        = MaxSlowTimer;
    CurrentEnemiesKilled    = 0;

    EnemiesGoalToSaySomething = static_cast<uint8_t>(FMath::RandRange(2, 3));

    UGameplayStatics::PlaySound2D(GetWorld(), Sound_HeartBeat);
    Sound_HeartBeat->VolumeMultiplier = 0.0f;

    LoadPlayerWeaponsFromSaveFile();
    StartUpdateWeaponHeat();
    StartRegenerateHealth();
    StartCheckEnemiesOnTarget();
    StartUpdateSlowTimer();
    StartCheckLowHealthFeedBack();
    SetNewWeaponHUD();
    UpdateWeaponSlotsHUD();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    CheckPostProcessEffects(DeltaTime);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::LoadPlayerWeaponsFromSaveFile()
{
    if (GameInstanceReference != nullptr)
    {
        const FName& AssaultRifleType = GameInstanceReference->OriginalSaveFile->AssaultRifleType;
        const FName& ShotgunType      = GameInstanceReference->OriginalSaveFile->ShotgunType;
        const FName& SniperType       = GameInstanceReference->OriginalSaveFile->SniperType;

        //Assault Rifle
        if (AssaultRifleType == SAVE_WEAPON_NORMAL)
        {
            SpawnAndAttachWeapon(EAllWeaponTypes::AssaultRifleWeapon, NAME_WeaponSocket, true, false);
        }
        else if (AssaultRifleType == SAVE_WEAPON_LEGENDARY)
        {
            SpawnAndAttachWeapon(EAllWeaponTypes::LegendaryAssaultRifle, NAME_WeaponSocket, true, false);
        }

        //Shotgun
        if (ShotgunType == SAVE_WEAPON_NORMAL)
        {
            SpawnAndAttachWeapon(EAllWeaponTypes::ShotgunWeapon, NAME_ShotgunBackSocket, false, false);
        }
        else if(ShotgunType == SAVE_WEAPON_LEGENDARY)
        {
            SpawnAndAttachWeapon(EAllWeaponTypes::LegendaryShotgun, NAME_ShotgunBackSocket, false, false);
        }

        //Sniper
        if (SniperType == SAVE_WEAPON_NORMAL)
        {
            SpawnAndAttachWeapon(EAllWeaponTypes::SniperRifleWeapon, NAME_SniperBackSocket, false, false);
        }
        else if (SniperType == SAVE_WEAPON_LEGENDARY)
        {
            SpawnAndAttachWeapon(EAllWeaponTypes::LegendarySniperRifle, NAME_SniperBackSocket, false, false);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
bool ATPS_PlayerCharacter::TakeNewWeapon(EAllWeaponTypes WeaponType)
{
    if (bIsSwitchingWeapons)
    {
        return false;
    }

    switch (WeaponType)
    {
        default:
            PlayerCanFireAndTakeWeapons(true);
            return false;
        break;

        case EAllWeaponTypes::AssaultRifleWeapon:
            //Is this slot empty?
            if (AssaultRifleSlot == nullptr)
            {
                //Spawn the weapon and attach it to the back.
                SpawnAndAttachWeapon(EAllWeaponTypes::AssaultRifleWeapon, NAME_RifleBackSocket, false, false);
                return true;
            }
            else //If this slot is not empty...
            {
                //Are we trying to take the same weapon that we already have?
                if (AssaultRifleSlot->ThisWeaponType == EAllWeaponTypes::AssaultRifleWeapon)
                {
                    //We don't do anything. We already have this weapon.
                    PlayerCanFireAndTakeWeapons(true); 
                    return false; 
                }
                //But, if this is a different version of the weapon...
                else
                {
                    //Is this weapon being equipped right now?
                    if (EquippedWeapon == AssaultRifleSlot) 
                    {
                        //Spawn the weapon, attach it to our hand, set the new weapon to the slot, and spawn a pickup weapon on the floor.
                        SpawnAndAttachWeapon(EAllWeaponTypes::AssaultRifleWeapon, NAME_WeaponSocket, true, true);
                    }
                    //This weapon is not being equipped right now. It's in the character's back,
                    else
                    {
                        SpawnAndAttachWeapon(EAllWeaponTypes::AssaultRifleWeapon, NAME_RifleBackSocket, false, true); //Spawn the weapon, attach it to our back, set the new weapon to the slot, and spawn a pickup weapon on the floor.
                    }

                    UGameplayStatics::PlaySound2D(GetWorld(), TakeWeaponSound);
                    return true;
                }
            }
        break;

        case EAllWeaponTypes::LegendaryAssaultRifle:
            if (AssaultRifleSlot == nullptr)
            {
                SpawnAndAttachWeapon(EAllWeaponTypes::LegendaryAssaultRifle, NAME_RifleBackSocket, false, false);
                UGameplayStatics::PlaySound2D(GetWorld(), TakeWeaponSound);
                return true;
            }
            else
            {
                if (AssaultRifleSlot->ThisWeaponType == EAllWeaponTypes::LegendaryAssaultRifle)
                {
                    PlayerCanFireAndTakeWeapons(true);
                    return false;
                }
                else
                {
                    if (EquippedWeapon == AssaultRifleSlot)
                    {
                        SpawnAndAttachWeapon(EAllWeaponTypes::LegendaryAssaultRifle, NAME_WeaponSocket, true, true);
                    }
                    else
                    {
                        SpawnAndAttachWeapon(EAllWeaponTypes::LegendaryAssaultRifle, NAME_RifleBackSocket, false, true);
                    }

                    UGameplayStatics::PlaySound2D(GetWorld(), TakeWeaponSound);
                    return true;
                }
            }
        break;

        case EAllWeaponTypes::ShotgunWeapon:
            if (ShotgunSlot == nullptr)
            {
                SpawnAndAttachWeapon(EAllWeaponTypes::ShotgunWeapon, NAME_ShotgunBackSocket, false, false);
                UGameplayStatics::PlaySound2D(GetWorld(), TakeWeaponSound);
                return true;
            }
            else
            {
                if (ShotgunSlot->ThisWeaponType == EAllWeaponTypes::ShotgunWeapon)
                {
                    PlayerCanFireAndTakeWeapons(true);
                    return false;
                }
                else
                {
                    if (EquippedWeapon == ShotgunSlot)
                    {
                        SpawnAndAttachWeapon(EAllWeaponTypes::ShotgunWeapon, NAME_WeaponSocket, true, true);
                    }
                    else
                    {
                        SpawnAndAttachWeapon(EAllWeaponTypes::ShotgunWeapon, NAME_ShotgunBackSocket, false, true);
                    }

                    UGameplayStatics::PlaySound2D(GetWorld(), TakeWeaponSound);
                    return true;
                }
            }
        break;

        case EAllWeaponTypes::LegendaryShotgun:
            if (ShotgunSlot == nullptr)
            {
                SpawnAndAttachWeapon(EAllWeaponTypes::LegendaryShotgun, NAME_ShotgunBackSocket, false, false);
                UGameplayStatics::PlaySound2D(GetWorld(), TakeWeaponSound);
                return true;
            }
            else
            {
                if (ShotgunSlot->ThisWeaponType == EAllWeaponTypes::LegendaryShotgun)
                {
                    PlayerCanFireAndTakeWeapons(true);
                    return false;
                }
                else
                {
                    if (EquippedWeapon == ShotgunSlot)
                    {
                        SpawnAndAttachWeapon(EAllWeaponTypes::LegendaryShotgun, NAME_WeaponSocket, true, true);
                    }
                    else
                    {
                        SpawnAndAttachWeapon(EAllWeaponTypes::LegendaryShotgun, NAME_ShotgunBackSocket, false, true);
                    }

                    UGameplayStatics::PlaySound2D(GetWorld(), TakeWeaponSound);
                    return true;
                }
            }
        break;

        case EAllWeaponTypes::SniperRifleWeapon:
            if (SniperRifleSlot == nullptr)
            {
                SpawnAndAttachWeapon(EAllWeaponTypes::SniperRifleWeapon, NAME_SniperBackSocket, false, false);
                
                UGameplayStatics::PlaySound2D(GetWorld(), TakeWeaponSound);
                return true;
            }
            else
            {
                if (SniperRifleSlot->ThisWeaponType == EAllWeaponTypes::SniperRifleWeapon)
                {
                    PlayerCanFireAndTakeWeapons(true);
                    return false;
                }
                else
                {
                    if (EquippedWeapon == SniperRifleSlot)
                    {
                        SpawnAndAttachWeapon(EAllWeaponTypes::SniperRifleWeapon, NAME_WeaponSocket, true, true);
                    }
                    else
                    {
                        SpawnAndAttachWeapon(EAllWeaponTypes::SniperRifleWeapon, NAME_SniperBackSocket, false, true);
                    }

                    UGameplayStatics::PlaySound2D(GetWorld(), TakeWeaponSound);
                    return true;
                }
            }
        break;

        case EAllWeaponTypes::LegendarySniperRifle:
            if (SniperRifleSlot == nullptr)
            {
                SpawnAndAttachWeapon(EAllWeaponTypes::LegendarySniperRifle, NAME_SniperBackSocket, false, false);

                UGameplayStatics::PlaySound2D(GetWorld(), TakeWeaponSound);
                return true;
            }
            else
            {
                if (SniperRifleSlot->ThisWeaponType == EAllWeaponTypes::LegendarySniperRifle)
                {
                    PlayerCanFireAndTakeWeapons(true);
                    return false;
                }
                else
                {
                    if (EquippedWeapon == SniperRifleSlot)
                    {
                        SpawnAndAttachWeapon(EAllWeaponTypes::LegendarySniperRifle, NAME_WeaponSocket, true, true);
                    }
                    else
                    {
                        SpawnAndAttachWeapon(EAllWeaponTypes::LegendarySniperRifle, NAME_SniperBackSocket, false, true);
                    }

                    UGameplayStatics::PlaySound2D(GetWorld(), TakeWeaponSound);
                    return true;
                }
            }
        break;
    }

    //It should never get here.
    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::EnemyKilledByPlayer()
{
    if (!bIsSlowTimeActivated)
    {
        CurrentSlowTimer += SlowTimerAddValue;
    }

    PlayerController->SetHitmarkHUD();
    ++CurrentEnemiesKilled;

    if (CurrentEnemiesKilled >= EnemiesGoalToSaySomething)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), KilledEnemySound, GetActorLocation());

        CurrentEnemiesKilled      = 0;
        EnemiesGoalToSaySomething = static_cast<uint8_t>(FMath::RandRange(3, 6));
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::BulletSpawnedFeedBack()
{
    if (CurrentWeapon == ECurrentWeapon::AssaultRifleWeapon ||
        CurrentWeapon == ECurrentWeapon::LegendaryAssaultRifle)
    {
        PlayerController->PlayRumbleFeedBack(ERumbleFeedBack::RifleFeedBack);
    }

    else if (CurrentWeapon == ECurrentWeapon::ShotgunWeapon ||
             CurrentWeapon == ECurrentWeapon::LegendaryShotgun)
    {
        PlayerController->PlayRumbleFeedBack(ERumbleFeedBack::ShotgunFeedBack);
    }

    else if (CurrentWeapon == ECurrentWeapon::SniperRifleWeapon ||
             CurrentWeapon == ECurrentWeapon::LegendarySniperWeapon)
    {
        PlayerController->PlayRumbleFeedBack(ERumbleFeedBack::SniperFeedBack);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::SpawnAndAttachWeapon(EAllWeaponTypes WeaponType, const FName& AttachToThisSocket,
                                                const bool& bEquipThisOne, const bool& bThrowWeapon)
{
    FActorSpawnParameters ActorSpawnParams;
    ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    ActorSpawnParams.Instigator	                    = this;

    ABaseWeapon* SpawnedWeapon = nullptr;
    PlayerCanFireAndTakeWeapons(false);

    switch (WeaponType)
    {
        case EAllWeaponTypes::AssaultRifleWeapon:
        {
            SpawnedWeapon = GetWorld()->SpawnActor<AAssaultRifle>(AssaultRifle, GetActorTransform(), ActorSpawnParams);
            GameInstanceReference->OriginalSaveFile->AssaultRifleType = SAVE_WEAPON_NORMAL;

            if (bThrowWeapon)
            {
                AssaultRifleSlot->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
                AssaultRifleSlot->DestroyThisWeapon();
            }

            AssaultRifleSlot = CastChecked<AAssaultRifle>(SpawnedWeapon);

            if (bEquipThisOne)
            {
                EquippedWeapon = SpawnedWeapon;
                CurrentWeapon  = ECurrentWeapon::AssaultRifleWeapon;
            }
        }
        break;

        case EAllWeaponTypes::LegendaryAssaultRifle:
        {
            SpawnedWeapon = GetWorld()->SpawnActor<AAssaultRifle>(LegendAssaultRifle, GetActorTransform(), ActorSpawnParams);
            GameInstanceReference->OriginalSaveFile->AssaultRifleType = SAVE_WEAPON_LEGENDARY;

            if (bThrowWeapon)
            {
                AssaultRifleSlot->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
                AssaultRifleSlot->DestroyThisWeapon();
            }

            AssaultRifleSlot = CastChecked<AAssaultRifle>(SpawnedWeapon);

            if (bEquipThisOne)
            {
                EquippedWeapon = SpawnedWeapon;
                CurrentWeapon  = ECurrentWeapon::LegendaryAssaultRifle;
            }
        }
        break;

        case EAllWeaponTypes::ShotgunWeapon:
        {
            SpawnedWeapon = GetWorld()->SpawnActor<AShotgun>(Shotgun, GetActorTransform(), ActorSpawnParams);
            GameInstanceReference->OriginalSaveFile->ShotgunType = SAVE_WEAPON_NORMAL;

            if (bThrowWeapon)
            {
                ShotgunSlot->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
                ShotgunSlot->DestroyThisWeapon();
            }

            ShotgunSlot = CastChecked<AShotgun>(SpawnedWeapon);

            if (bEquipThisOne)
            {
                EquippedWeapon = SpawnedWeapon;
                CurrentWeapon  = ECurrentWeapon::ShotgunWeapon;
            }
        }
        break;

        case EAllWeaponTypes::LegendaryShotgun:
        {
            SpawnedWeapon = GetWorld()->SpawnActor<AShotgun>(LegendShotgun, GetActorTransform(), ActorSpawnParams);
            GameInstanceReference->OriginalSaveFile->ShotgunType = SAVE_WEAPON_LEGENDARY;

            if (bThrowWeapon)
            {
                ShotgunSlot->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
                ShotgunSlot->DestroyThisWeapon();
            }

            ShotgunSlot = CastChecked<AShotgun>(SpawnedWeapon);

            if (bEquipThisOne)
            {
                EquippedWeapon = SpawnedWeapon;
                CurrentWeapon  = ECurrentWeapon::LegendaryShotgun;
            }
        }
        break;

        case EAllWeaponTypes::SniperRifleWeapon:
        {
            SpawnedWeapon = GetWorld()->SpawnActor<ASniperRifleWeapon>(Sniper, GetActorTransform(), ActorSpawnParams);
            GameInstanceReference->OriginalSaveFile->SniperType = SAVE_WEAPON_NORMAL;

            if (bThrowWeapon)
            {
                SniperRifleSlot->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
                SniperRifleSlot->DestroyThisWeapon();
            }

            SniperRifleSlot = CastChecked<ASniperRifleWeapon>(SpawnedWeapon);

            if (bEquipThisOne)
            {
                EquippedWeapon = SpawnedWeapon;
                CurrentWeapon  = ECurrentWeapon::SniperRifleWeapon;
            }
        }
        break;

        case EAllWeaponTypes::LegendarySniperRifle:
        {
            SpawnedWeapon = GetWorld()->SpawnActor<ASniperRifleWeapon>(LegendSniper, GetActorTransform(), ActorSpawnParams);
            GameInstanceReference->OriginalSaveFile->SniperType = SAVE_WEAPON_LEGENDARY;

            if (bThrowWeapon)
            {
                SniperRifleSlot->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
                SniperRifleSlot->DestroyThisWeapon();
            }

            SniperRifleSlot = CastChecked<ASniperRifleWeapon>(SpawnedWeapon);

            if (bEquipThisOne)
            {
                EquippedWeapon = SpawnedWeapon;
                CurrentWeapon  = ECurrentWeapon::LegendarySniperWeapon;
            }
        }
        break;

        default:
        break;
    }

    SpawnedWeapon->bIsPlayerWeapon = true;
    SpawnedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachToThisSocket);
    SpawnedWeapon->SetCamera(CameraComponent);

    PlayerCanFireAndTakeWeapons(true);
    SetNewWeaponHUD();
    UpdateWeaponSlotsHUD();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::SetIsTakingCover(bool bIsTakingCover)
{
    bPlayerIsTakingCover      = bIsTakingCover;
    bUseControllerRotationYaw = !bPlayerIsTakingCover;
    CurrentFOVValue           = bPlayerIsTakingCover ? CoverFOVValue : DefaultFOVValue;

    if (LastCoverReference != nullptr)
    { 
        LastCoverReference->SetCoverIsUsed(bPlayerIsTakingCover); 
    }

    if (bPlayerIsTakingCover)
    {
        SetActorRotation(_CoverRotation);
        PlayerController->SetIsSprinting(false);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::SetCanFire(bool bCanFire)
{
    bPlayerCanFire = bCanFire;
    if (!bPlayerCanFire)
    {
        StopFireWeapon();
    }

    PlayerController->SetCrossHairVisibility(bPlayerCanFire);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::TakeCover()
{
    if (bPlayerIsTakingCover)
    {
        SetIsTakingCover(false);
    }
    else
    {
        SetIsTakingCover(bCoverAvailable && CheckCoverAngle());
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::FireWeapon()
{
    if (bPlayerCanFire)
    {
        PlayerController->SetCrossHairVisibility(true);
        EquippedWeapon->SetFasterBulletOnSlowTime(bIsSlowTimeActivated, PlayerCustomTimeDilation);
        EquippedWeapon->FireWeapon();
        NoiseEmitter->MakeNoise(this, 2.5f, GetActorLocation());
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::StopFireWeapon()
{
    EquippedWeapon->SetFasterBulletOnSlowTime(bIsSlowTimeActivated, PlayerCustomTimeDilation);
    EquippedWeapon->StopFireWeapon();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::PlayerStartsMeleeAttack()
{
    if (!bPlayerOnMeleeAttack)
    {
        PlayerController->SetCrossHairVisibility(false);
        //Stop shooting
        EquippedWeapon->StopFireWeapon();
        //Stop taking cover
        SetIsTakingCover(false); 
        //You can't fire or take weapons
        PlayerCanFireAndTakeWeapons(false); 
        //Executes physic attack animation.
        bPlayerOnMeleeAttack = true; 
        //Stop SlowTime if it is enable.
        PlayerStopSlowTimeBullet(); 
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::PlayerEndsMeleeAttack()
{
    PlayerCanFireAndTakeWeapons(true);
    bPlayerOnMeleeAttack = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::OnMeleeHit()
{
    FHitResult HitResult;

    const FVector& StartTrace = GetActorLocation();
    const FVector& EndTrace   = StartTrace + (GetMesh()->GetRightVector() * MeleeDistance);

    TArray<AActor*> IgnoreThis;
    IgnoreThis.Add(this);

    //Make a BoxTrace to check if the enemy is in front of us.
    const bool BoxTrace =  UKismetSystemLibrary::BoxTraceSingle(GetWorld(), StartTrace, EndTrace,
                           FVector(1.0f, 30.0f, 30.0f), FRotator(0.0f, 0.0f, 0.0f), (ETraceTypeQuery)WEAPON_TRACE,
                           true, IgnoreThis, EDrawDebugTrace::None, HitResult, true);

    if (BoxTrace)
    {
        if (UKismetSystemLibrary::DoesImplementInterface(HitResult.GetActor(), UDamageInterface::StaticClass()))
        {
            IDamageInterface::Execute_TakeDamage(HitResult.GetActor(), MinMeleeDamage, MaxMeleeDamage, CriticChanceMeleeDamage, HitResult.BoneName, this->GetInstigator());
        
            PlayerController->PlayRumbleFeedBack(ERumbleFeedBack::MeleeFeedBack);
            UGameplayStatics::PlaySoundAtLocation(GetWorld(), MeleeHit, GetActorLocation());
            UGameplayStatics::PlayWorldCameraShake(GetWorld(), MeleeCameraShake, GetActorLocation(), 0.0f, 500.0f, 1.0f, false);
            NoiseEmitter->MakeNoise(this, 2.5f, GetActorLocation());
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::SwitchWeapons(EAllWeaponTypes SwitchToThis)
{
    bIsSwitchingWeapons = true;
    StopFireWeapon();

    if (!bPlayerCanSwitchWeapons)
    {
        return;
    }

    PlayerCanFireAndTakeWeapons(false);

    switch (SwitchToThis)
    {
        case EAllWeaponTypes::AssaultRifleWeapon:
        if(AssaultRifleSlot == nullptr)
        {
            bIsSwitchingWeapons = false;
            PlayerCanFireAndTakeWeapons(true);
            return; 
        }

        if (EquippedWeapon != AssaultRifleSlot)
        {
            SetFOV(false);
            FName AttachHere = CurrentWeapon == ECurrentWeapon::ShotgunWeapon ||
                               CurrentWeapon == ECurrentWeapon::LegendaryShotgun ? NAME_ShotgunBackSocket : NAME_SniperBackSocket;

            EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachHere);
            EquippedWeapon = CastChecked<ABaseWeapon>(AssaultRifleSlot);
            EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, NAME_WeaponSocket);

            CurrentWeapon = (AssaultRifleSlot->ThisWeaponType == EAllWeaponTypes::AssaultRifleWeapon) ? ECurrentWeapon::AssaultRifleWeapon : ECurrentWeapon::LegendaryAssaultRifle;

            UGameplayStatics::PlaySound2D(GetWorld(), SwitchToRifle);
        }
        break;

        case EAllWeaponTypes::ShotgunWeapon:
            if (ShotgunSlot == nullptr)
            {
                bIsSwitchingWeapons = false;
                PlayerCanFireAndTakeWeapons(true);
                return;
            }

            if (EquippedWeapon != ShotgunSlot)
            {
                SetFOV(false);
                FName AttachHere = CurrentWeapon == ECurrentWeapon::AssaultRifleWeapon ||
                                   CurrentWeapon == ECurrentWeapon::LegendaryAssaultRifle ? NAME_RifleBackSocket : NAME_SniperBackSocket;

                EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachHere);
                EquippedWeapon = CastChecked<ABaseWeapon>(ShotgunSlot);
                EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, NAME_WeaponSocket);

                CurrentWeapon = ShotgunSlot->ThisWeaponType == EAllWeaponTypes::ShotgunWeapon ? ECurrentWeapon::ShotgunWeapon : ECurrentWeapon::LegendaryShotgun;
                UGameplayStatics::PlaySound2D(GetWorld(), SwitchToShotgun);
            }
        break;

        case EAllWeaponTypes::SniperRifleWeapon:
            if (SniperRifleSlot == nullptr)
            {
                bIsSwitchingWeapons = false;
                PlayerCanFireAndTakeWeapons(true);
                return;
            }

            if (EquippedWeapon != SniperRifleSlot)
            {
                SetFOV(false);
                FName AttachHere = CurrentWeapon == ECurrentWeapon::AssaultRifleWeapon ||
                                   CurrentWeapon == ECurrentWeapon::LegendaryAssaultRifle ? NAME_RifleBackSocket : NAME_ShotgunBackSocket;

                EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachHere);
                EquippedWeapon = CastChecked<ABaseWeapon>(SniperRifleSlot);
                EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, NAME_WeaponSocket);

                CurrentWeapon = SniperRifleSlot->ThisWeaponType == EAllWeaponTypes::SniperRifleWeapon ? ECurrentWeapon::SniperRifleWeapon : ECurrentWeapon::LegendarySniperWeapon;
                UGameplayStatics::PlaySound2D(GetWorld(), SwitchToSniper);
            }
        break;
    }

    bIsSwitchingWeapons = false;
    PlayerCanFireAndTakeWeapons(true);
    SetNewWeaponHUD();
    UpdateSelectedWeaponHUD();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::ForceWeaponCoolDown(const bool& bForceWeaponCoolDown)
{
    if (bForceWeaponCoolDown)
    { 
        EquippedWeapon->ForceCoolDownWeapon(); 
    }
    else
    { 
        EquippedWeapon->StopForceCoolDownWeapon(); 
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
bool ATPS_PlayerCharacter::CheckCoverAngle()
{
    float DotProduct =  UKismetMathLibrary::Dot_VectorVector
                       (UKismetMathLibrary::GetForwardVector(GetActorRotation()),
                        UKismetMathLibrary::GetForwardVector(_CoverRotation));

    const float FinalAngle = UKismetMathLibrary::DegAcos(DotProduct);

    return FinalAngle < 55.0f;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::StartRegenerateHealth()
{
    GetWorldTimerManager().SetTimer(RegenerateHealth_TimerHandle, this, &ATPS_PlayerCharacter::AddHealth, 0.1f, true, 1.0f);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::StartUpdateWeaponHeat()
{
    GetWorldTimerManager().SetTimer(WeaponHeat_TimerHandle, this, &ATPS_PlayerCharacter::PerformUpdateHeatValue, 0.1f, true, 1.0f);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::StartUpdateSlowTimer()
{
    GetWorldTimerManager().SetTimer(SlowTimer_TimeHandle, this, &ATPS_PlayerCharacter::ConsumeSlowTimeBullet, 0.1f, true, 1.0f);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::StartCheckEnemiesOnTarget()
{
    GetWorldTimerManager().SetTimer(EnemyTarget_TimerHandle, this, &ATPS_PlayerCharacter::PerformUpdateEnemyOnTarget, 0.25f, true, 0.25f);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::StartCheckLowHealthFeedBack()
{
    GetWorldTimerManager().SetTimer(LowHealthFB_TimeHandle, this, &ATPS_PlayerCharacter::LowHealthRumbleFeedBack, 0.5f, true, 0.0f);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::CalculateHealth(const float& MinHealthDamage, const float& MaxHealthDamage)
{
    float LocalDamage = UKismetMathLibrary::RandomFloatInRange(MinHealthDamage, MaxHealthDamage);
    if (bPlayerIsTakingCover)
    {
        LocalDamage = LocalDamage * CoverDamageModifier;
    }

    //If we have less than 30% of life, we're gonna take just half of the damage.
    if (CurrentHealth <= LIFE_WARNING_PERCENT)
    {
        LocalDamage = LocalDamage / 2;
    }

    CurrentHealth = CurrentHealth - LocalDamage;
    if (CurrentHealth <= 0.0f)
    {
        CurrentHealth = 0.0f;
        OnPlayersDeath();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::PerformUpdateHealth()
{
    PlayerController->SetLifeHUDValue(CurrentHealth / MaxHealth);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::PlayerCanFireAndTakeWeapons(const bool& bPlayerCan)
{
    bPlayerCanSwitchWeapons = bPlayerCan;
    bPlayerCanFire          = bPlayerCan;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::PlayerSlowTimeBullet()
{
    if (!bIsSlowTimeActivated && CurrentSlowTimer > 0.0f)
    {
        PlayerController->SetCrossHairVisibility(true);
        UGameplayStatics::SetGlobalTimeDilation(GetWorld(), SlowWorldTimeDilation);
        UGameplayStatics::PlaySound2D(GetWorld(), SlowTimeActivated);

        this->CustomTimeDilation = PlayerCustomTimeDilation;
        bIsSlowTimeActivated     = true;
    }
    else if (bIsSlowTimeActivated || CurrentSlowTimer <= 0.0f)
    {
        PlayerStopSlowTimeBullet();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::PlayerStopSlowTimeBullet()
{
    UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);

    this->CustomTimeDilation = 1.0f;
    bIsSlowTimeActivated     = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::SetNewWeaponHUD()
{
    PlayerController->SetCurrentWeaponHUD((uint8)CurrentWeapon);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::UpdateWeaponSlotsHUD()
{
    PlayerController->SetWeaponSlotsHUD();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::UpdateSelectedWeaponHUD()
{
    UpdateWeaponSlotsHUD();
    PlayerController->ShowWeaponSwitchHUD();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::CheckPostProcessEffects(float DeltaTimer)
{
    HandleLowHealthEffects(DeltaTimer);
    HandleSlowTimePP(DeltaTimer);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::HandleLowHealthEffects(float DeltaTimerRef)
{
    //On SlowTime, we don't want to play this effect. Slow time has more priority.
    if (bIsSlowTimeActivated)
    {
        LowHealthPostProcess->ToggleVisibility(false);
    }
    else
    {
        LowHealthPostProcess->ToggleVisibility(true);
        LowHealthPostProcess->BlendWeight = 1.0f - ((CurrentHealth - 10.0f) / 40.0f);

        if ((CurrentHealth <= 40.0f) && (!bIsDead))
        {
            Sound_HeartBeat->VolumeMultiplier = 5.0f;
            Sound_HeartBeat->PitchMultiplier  = 1.0f - ((CurrentHealth - 10.0f) / 40.0f);
        }
        else
        {
            Sound_HeartBeat->VolumeMultiplier = 0.0f;
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::HandleSlowTimePP(float DeltaTimerRef)
{
    if (bIsSlowTimeActivated)
    {
        if (SlowTimePostProcess->BlendWeight < 1.0f)
        {
            float CurrentBlendWeight         = SlowTimePostProcess->BlendWeight;
            SlowTimePostProcess->BlendWeight = FMath::FInterpConstantTo(CurrentBlendWeight, 1.0f, DeltaTimerRef, PP_SlowTime_Speed);
        }
        else
        {
            SlowTimePostProcess->BlendWeight = 1.0f;
        }
    }
    else
    {
        if (SlowTimePostProcess->BlendWeight > 0.0f)
        {
            float CurrentBlendWeight         = SlowTimePostProcess->BlendWeight;
            SlowTimePostProcess->BlendWeight = FMath::FInterpConstantTo(CurrentBlendWeight, 0.0f, DeltaTimerRef, PP_SlowTime_Speed);
        }
        else
        {
            SlowTimePostProcess->BlendWeight = 0.0f;
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::DamageVFX(const FName& BoneName)
{
    const int32& SelectIndex = UKismetMathLibrary::RandomIntegerInRange(0, BloodFloor.Num() - 1);

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    SpawnParams.bNoFail                        = true;

    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodVFX, GetMesh()->GetBoneLocation(BoneName), FRotator::ZeroRotator, FVector(0.5f, 0.5f, 0.5f), true, EPSCPoolMethod::AutoRelease, true);
    GetWorld()->SpawnActor<ADecalActor>(BloodFloor[SelectIndex], GetActorTransform(), SpawnParams);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::OnPlayersDeath()
{
    bIsDead = true;
    Sound_HeartBeat->VolumeMultiplier = 0.0f;

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->SetSimulatePhysics(true);
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

    PlayerController->PlayerIsDead();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::EndDamageTimer()
{
    bIsTakingDamage = false;
    GetWorldTimerManager().ClearTimer(Damage_TimerHandle);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::AddHealth()
{
    if (!bIsDead && !bIsTakingDamage && CurrentHealth < MaxHealth)
    {
        CurrentHealth = CurrentHealth + HealthRegenerationValue;
        CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);
        PerformUpdateHealth();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::PerformUpdateHeatValue()
{
    if (EquippedWeapon != nullptr)
    {
        PlayerController->SetWeaponHeatHUDValue(EquippedWeapon->ReturnHeatValue());
        PlayerController->SetWeaponIsOverHeatedHUDValue(EquippedWeapon->WeaponIsOverHeatedValue());
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::PerformUpdateEnemyOnTarget()
{
    if(EquippedWeapon == nullptr) 
    {
        return;
    }

    FHitResult HitResult;
    bool bOnTargetEnemy = false;

    const FVector StartTrace = CameraComponent->GetComponentLocation();
    const FVector EndTrace   = (CameraComponent->GetForwardVector() * EquippedWeapon->ShootLength) + StartTrace;

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    QueryParams.bTraceComplex = false;

    if (GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECollisionChannel::ECC_Pawn, QueryParams))
    {
        if (AShooterEnemy* Enemy = Cast<AShooterEnemy>(HitResult.GetActor()))
        {
            if (!Enemy->GetIfEnemyIsDead())
            {
                bOnTargetEnemy = true;
            }
        }
    }

    PlayerController->SetCrossHairColor(bOnTargetEnemy);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::ConsumeSlowTimeBullet()
{
    if (bIsSlowTimeActivated)
    {
        CurrentSlowTimer -= SlowTimerConsumeValue;

        if (CurrentSlowTimer <= 0.0f)
        {
            CurrentSlowTimer = 0.0f;
            PlayerSlowTimeBullet();
        }
    }

    PlayerController->SetSlowTimeHUDValue(CurrentSlowTimer / MaxSlowTimer);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::LowHealthRumbleFeedBack()
{
    if (bIsDead)
    {
        return;
    }

    if (CurrentHealth <= 30.0f)
    {
        PlayerController->PlayRumbleFeedBack(ERumbleFeedBack::LowHealthFeedBack);
    }
}

#pragma region INTERFACES

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::EndOfCover_Implementation(bool bEndOfCoverLeft, bool bEndOfCoverRight)
{
    bPlayerEndOfCoverLeft  = bEndOfCoverLeft;
    bPlayerEndOfCoverRight = bEndOfCoverRight;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::IsCoverAvailable_Implementation(bool bIsCoverAvailable, FRotator CoverRotation)
{
    bCoverAvailable = bIsCoverAvailable;
    _CoverRotation  = CoverRotation;
    if (!bCoverAvailable)
    {
        SetIsTakingCover(false);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerCharacter::TakeDamage_Implementation(float DamageMin, float DamageMax, float CriticChance, FName HitBoneName, AActor* DamageInstigator)
{
    if (bIsDead)
    {
        return;
    }
    CalculateHealth(DamageMin, DamageMax);
    DamageVFX(HitBoneName);

    bIsTakingDamage = true;
    GetWorldTimerManager().SetTimer(Damage_TimerHandle, this, &ATPS_PlayerCharacter::EndDamageTimer, EndDamageTimerRate, false, EndDamageTimerRate);
    PerformUpdateHealth();

    UGameplayStatics::PlaySoundAtLocation(GetWorld(), TakeDamageSound, GetActorLocation());
    PlayerController->PlayRumbleFeedBack(ERumbleFeedBack::TakeDamageFeedBack);
}

#pragma endregion

#undef SAVE_WEAPON_NORMAL
#undef SAVE_WEAPON_LEGENDARY
#undef LIFE_WARNING_PERCENT