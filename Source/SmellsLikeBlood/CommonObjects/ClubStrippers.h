//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SmellsLikeBlood/Interfaces/DamageInterface.h"

#include "ClubStrippers.generated.h"

UCLASS()
class SMELLSLIKEBLOOD_API AClubStrippers : public AActor, public IDamageInterface
{
    GENERATED_BODY()

    //*******************************************************************************************************************
    //                                         COMPONENTS                                                               *
    //*******************************************************************************************************************

    UPROPERTY(VisibleAnywhere, Category = Components, meta = (AllowPrivateAccess = "true"))
    class USkeletalMeshComponent* BodySKMesh;

public:

    //Constructor
    AClubStrippers();

    //*******************************************************************************************************************
    //                                          INTERFACES FUNCTIONS                                                    *
    //*******************************************************************************************************************

    void TakeDamage_Implementation(float DamageMin, float DamageMax, float CriticChance, FName HitBoneName, AActor* DamageInstigator) override;

private:

    //*******************************************************************************************************************
    //                                          PRIVATE VARIABLES                                                       *
    //*******************************************************************************************************************

    class ATPS_PlayerController* PlayerController_Ref;

    UPROPERTY(EditDefaultsOnly, Category = "VFX")
    UParticleSystem* BloodVFX;

    //*******************************************************************************************************************
    //                                          PRIVATE FUNCTIONS                                                       *
    //*******************************************************************************************************************

    virtual void BeginPlay() override;

};