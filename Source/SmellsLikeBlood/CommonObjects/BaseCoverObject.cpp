//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#include "BaseCoverObject.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

#include "Kismet/GameplayStatics.h"

#include "SmellsLikeBlood/Enemies/ShooterEnemy.h"
#include "SmellsLikeBlood/Player/TPS_PlayerCharacter.h"

ABaseCoverObject::ABaseCoverObject()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SceneRoot->Mobility = EComponentMobility::Static;
    RootComponent = SceneRoot;

    CoverObject = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoverObject"));
    CoverObject->SetGenerateOverlapEvents(true);
    CoverObject->SetupAttachment(RootComponent);
    CoverObject->Mobility = EComponentMobility::Static;
    CoverObject->SetCollisionObjectType(ECC_WorldStatic);

    CoverBox1 = CreateDefaultSubobject<UBoxComponent>(TEXT("CoverBox1"));
    CoverBox2 = CreateDefaultSubobject<UBoxComponent>(TEXT("CoverBox2"));
    CoverBox1->SetupAttachment(RootComponent);
    CoverBox2->SetupAttachment(RootComponent);

    Arrow1 = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow1"));
    Arrow2 = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow2"));
    Arrow1->SetupAttachment(CoverBox1);
    Arrow2->SetupAttachment(CoverBox2);

    EndOfCover_1_Right = CreateDefaultSubobject<UBoxComponent>(TEXT("End Of Cover 1 Right"));
    EndOfCover_2_Right = CreateDefaultSubobject<UBoxComponent>(TEXT("End Of Cover 2 Right"));
    EndOfCover_1_Left  = CreateDefaultSubobject<UBoxComponent>(TEXT("End Of Cover 1 Left"));
    EndOfCover_2_Left  = CreateDefaultSubobject<UBoxComponent>(TEXT("End Of Cover 2 Left"));
    
    EndOfCover_1_Right->SetupAttachment(CoverBox1);
    EndOfCover_2_Right->SetupAttachment(CoverBox2);
    EndOfCover_1_Left->SetupAttachment(CoverBox1);
    EndOfCover_2_Left->SetupAttachment(CoverBox2);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseCoverObject::SetCoverIsUsed(const bool& bCoverUsed)
{
    bCoverIsBeingUsed = bCoverUsed;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseCoverObject::BeginPlay()
{
    CoverBox1->OnComponentBeginOverlap.AddDynamic(this, &ABaseCoverObject::CoverBox_One_BeginOverlap);
    CoverBox2->OnComponentBeginOverlap.AddDynamic(this, &ABaseCoverObject::CoverBox_Two_BeginOverlap);
    CoverBox1->OnComponentEndOverlap.AddDynamic(this,   &ABaseCoverObject::CoverBoxEndOverlap);
    CoverBox2->OnComponentEndOverlap.AddDynamic(this,   &ABaseCoverObject::CoverBoxEndOverlap);

    EndOfCover_1_Left->OnComponentBeginOverlap.AddDynamic(this, &ABaseCoverObject::EndOfCoverBox_Left_BeginOverlap);
    EndOfCover_2_Left->OnComponentBeginOverlap.AddDynamic(this, &ABaseCoverObject::EndOfCoverBox_Left_BeginOverlap);

    EndOfCover_1_Right->OnComponentBeginOverlap.AddDynamic(this, &ABaseCoverObject::EndOfCoverBox_Right_BeginOverlap);
    EndOfCover_2_Right->OnComponentBeginOverlap.AddDynamic(this, &ABaseCoverObject::EndOfCoverBox_Right_BeginOverlap);

    EndOfCover_1_Left->OnComponentEndOverlap.AddDynamic(this,  &ABaseCoverObject::EndOfCoverBox_EndOverlap);
    EndOfCover_2_Left->OnComponentEndOverlap.AddDynamic(this,  &ABaseCoverObject::EndOfCoverBox_EndOverlap);
    EndOfCover_1_Right->OnComponentEndOverlap.AddDynamic(this, &ABaseCoverObject::EndOfCoverBox_EndOverlap);
    EndOfCover_2_Right->OnComponentEndOverlap.AddDynamic(this, &ABaseCoverObject::EndOfCoverBox_EndOverlap);
}

#pragma region OVERLAP FUNCTIONS

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseCoverObject::CoverBox_One_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    TellPlayerIfCoverIsAvailable(OtherActor, true, Arrow1->GetComponentRotation());
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseCoverObject::CoverBox_Two_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    TellPlayerIfCoverIsAvailable(OtherActor, true, Arrow2->GetComponentRotation());
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseCoverObject::EndOfCoverBox_Left_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    TellPlayerIsEndOfCover(OtherActor, true, false);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseCoverObject::EndOfCoverBox_Right_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    TellPlayerIsEndOfCover(OtherActor, false, true);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseCoverObject::CoverBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    TellPlayerIfCoverIsAvailable(OtherActor, false, FRotator(0.0f, 0.0f, 0.0f));
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseCoverObject::EndOfCoverBox_EndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    TellPlayerIsEndOfCover(OtherActor, false, false);
}

#pragma endregion

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseCoverObject::TellPlayerIfCoverIsAvailable(AActor* CheckThisActor, bool bIsTheCoverAvailable, FRotator CoverRotation)
{
    if (UKismetSystemLibrary::DoesImplementInterface(CheckThisActor, UCoverInterface::StaticClass()))
    {
        if (ATPS_PlayerCharacter* PlayerCharacter = Cast<ATPS_PlayerCharacter>(CheckThisActor))
        {
            PlayerCharacter->SetLastCoverReference(this);
        }
        else if(AShooterEnemy* Enemy = Cast<AShooterEnemy>(CheckThisActor))
        {
            Enemy->SetLastCoverReference(this);
        }

        ICoverInterface::Execute_IsCoverAvailable(CheckThisActor, bIsTheCoverAvailable, CoverRotation);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ABaseCoverObject::TellPlayerIsEndOfCover(AActor* CheckThisActor, bool bEndOfCoverLeft, bool bEndOfCoverRight)
{
    if (UKismetSystemLibrary::DoesImplementInterface(CheckThisActor, UCoverInterface::StaticClass()))
    {
        ICoverInterface::Execute_EndOfCover(CheckThisActor, bEndOfCoverLeft, bEndOfCoverRight);
    }
}