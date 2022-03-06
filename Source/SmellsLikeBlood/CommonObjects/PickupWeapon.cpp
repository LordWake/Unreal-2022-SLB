//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#include "PickupWeapon.h"

#include "Components/PointLightComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"

#include "Kismet/GameplayStatics.h"

#include "SmellsLikeBlood/Player/TPS_PlayerController.h"
#include "SmellsLikeBlood/Player/TPS_PlayerCharacter.h"
#include "SmellsLikeBlood/Weapons/AssaultRifle.h"
#include "SmellsLikeBlood/Weapons/Shotgun.h"
#include "SmellsLikeBlood/Weapons/SniperRifleWeapon.h"

#include "Sound/SoundCue.h"

APickupWeapon::APickupWeapon()
{
    PickupWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PickupMesh"));
    PickupWeaponMesh->SetGenerateOverlapEvents(false);
    PickupWeaponMesh->SetSimulatePhysics(true);
    PickupWeaponMesh->SetCastShadow(false);

    SphereOverlapComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereOverlapComp"));
    SphereOverlapComponent->SetGenerateOverlapEvents(false);
    SphereOverlapComponent->SetupAttachment(PickupWeaponMesh);
    SphereOverlapComponent->SetSphereRadius(100.f);

    WeaponPointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("WeaponPointLight"));
    WeaponPointLight->SetupAttachment(PickupWeaponMesh);
    WeaponPointLight->SetIntensity(500.0f);
    WeaponPointLight->SetAttenuationRadius(50.0f);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APickupWeapon::OnPlayerTakeWeapon()
{
    UGameplayStatics::SpawnSound2D(GetWorld(), TakeWeaponSound, 1.f, 1.f, 0.f, nullptr, false, true);
    Destroy(true);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APickupWeapon::OnPlayerAboveWeapon(const bool bIsAbove)
{
    if (bIsAbove)
    {
        PlayerControllerReference->SetWeaponAvailableToPick(bIsAbove, ThisWeaponType, this);
    }
    else
    {
        PlayerControllerReference->SetWeaponAvailableToPick(bIsAbove, EAllWeaponTypes::None, this);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APickupWeapon::EnableOverlapEventsAfterSpawn()
{
    SphereOverlapComponent->SetGenerateOverlapEvents(true);
    GetWorldTimerManager().ClearTimer(TimerHandle_OverlapEnable);

    SphereOverlapComponent->OnComponentBeginOverlap.AddDynamic(this, &APickupWeapon::SphereBeginOverlap);
    SphereOverlapComponent->OnComponentEndOverlap.AddDynamic(this,   &APickupWeapon::SphereEndOverlap);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APickupWeapon::SphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (ATPS_PlayerCharacter* PlayerCharacter = Cast<ATPS_PlayerCharacter>(OtherActor))
    {
        bool bIsPossibleToTakeThisWeapon = true;

        switch (ThisWeaponType)
        {
            case EAllWeaponTypes::AssaultRifleWeapon:
                if (PlayerCharacter->AssaultRifleSlot->ThisWeaponType == EAllWeaponTypes::AssaultRifleWeapon)
                {
                    bIsPossibleToTakeThisWeapon = false;
                }
            break;
            case EAllWeaponTypes::LegendaryAssaultRifle:
                if (PlayerCharacter->AssaultRifleSlot->ThisWeaponType == EAllWeaponTypes::LegendaryAssaultRifle)
                {
                    bIsPossibleToTakeThisWeapon = false;
                }
            break;
            case EAllWeaponTypes::ShotgunWeapon:
                if (PlayerCharacter->ShotgunSlot != nullptr)
                {
                    if (PlayerCharacter->ShotgunSlot->ThisWeaponType == EAllWeaponTypes::ShotgunWeapon)
                    {
                        bIsPossibleToTakeThisWeapon = false;
                    }
                }
            break;
            case EAllWeaponTypes::LegendaryShotgun:
                if (PlayerCharacter->ShotgunSlot != nullptr)
                {
                    if (PlayerCharacter->ShotgunSlot->ThisWeaponType == EAllWeaponTypes::LegendaryShotgun)
                    {
                        bIsPossibleToTakeThisWeapon = false;
                    }
                }
            break;
            case EAllWeaponTypes::SniperRifleWeapon:
                if (PlayerCharacter->SniperRifleSlot != nullptr)
                {
                    if (PlayerCharacter->SniperRifleSlot->ThisWeaponType == EAllWeaponTypes::SniperRifleWeapon)
                    {
                        bIsPossibleToTakeThisWeapon = false;
                    }
                }
            break;
            case EAllWeaponTypes::LegendarySniperRifle:
                if (PlayerCharacter->SniperRifleSlot != nullptr)
                {
                    if (PlayerCharacter->SniperRifleSlot->ThisWeaponType == EAllWeaponTypes::LegendarySniperRifle)
                    {
                        bIsPossibleToTakeThisWeapon = false;
                    }
                }
            break;
        }

        OnPlayerAboveWeapon(bIsPossibleToTakeThisWeapon);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APickupWeapon::SphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (ATPS_PlayerCharacter* PlayerCharacter = Cast<ATPS_PlayerCharacter>(OtherActor))
    {
        OnPlayerAboveWeapon(false);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void APickupWeapon::BeginPlay()
{
    Super::BeginPlay();

    PlayerControllerReference = Cast<ATPS_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    GetWorldTimerManager().SetTimer(TimerHandle_OverlapEnable, this, &APickupWeapon::EnableOverlapEventsAfterSpawn, OverlapEnableTimer, false);
}