//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#include "ClubStrippers.h"

#include "Components/SkeletalMeshComponent.h"

#include "Kismet/GameplayStatics.h"

#include "SmellsLikeBlood/Player/TPS_PlayerController.h"
#include "SmellsLikeBlood/Player/TPS_PlayerCharacter.h"

AClubStrippers::AClubStrippers()
{
    PrimaryActorTick.bCanEverTick = true;

    BodySKMesh = CreateDefaultSubobject<USkeletalMeshComponent>("BodyMesh");
    SetRootComponent(BodySKMesh);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AClubStrippers::TakeDamage_Implementation(float DamageMin, float DamageMax, float CriticChance, FName HitBoneName, AActor* DamageInstigator)
{
    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodVFX, BodySKMesh->GetBoneLocation(HitBoneName), FRotator::ZeroRotator, FVector(1.f), true, EPSCPoolMethod::AutoRelease, true);

    auto GetControllerLambda =
    [&]()
    {
        PlayerController_Ref = CastChecked<ATPS_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    };

    if (Cast<ATPS_PlayerCharacter>(DamageInstigator))
    {
        if (PlayerController_Ref == nullptr)
        {
            GetControllerLambda();
        }

        PlayerController_Ref->StrippersDialogueWarning();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AClubStrippers::BeginPlay()
{
    Super::BeginPlay();
    PlayerController_Ref = CastChecked<ATPS_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}