//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SmellsLikeBlood/SmellsLikeBlood.h"
#include "PickupWeapon.generated.h"

UCLASS()
class SMELLSLIKEBLOOD_API APickupWeapon : public AActor
{
    GENERATED_BODY()

public:

    //*******************************************************************************************************************
    //                                          CONSTRUCTOR && PUBLIC VARIABLES                                         *
    //*******************************************************************************************************************

    //Constructor
    APickupWeapon();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Type") 
    EAllWeaponTypes ThisWeaponType;

    //*******************************************************************************************************************
    //                                          PUBLIC FUNCTIONS                                                        *
    //*******************************************************************************************************************

    /*Plays a sound and destroy this actor.*/
    void OnPlayerTakeWeapon();

protected:

    //*******************************************************************************************************************
    //                                          PROTECTED VARIABLES                                                     *
    //*******************************************************************************************************************

    FTimerHandle TimerHandle_OverlapEnable;

    class ATPS_PlayerController* PlayerControllerReference;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
    class USkeletalMeshComponent* PickupWeaponMesh;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lights", meta = (AllowPrivateAccess = "true"))
    class UPointLightComponent* WeaponPointLight;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Physics", meta = (AllowPrivateAccess = "true"))
    class USphereComponent* SphereOverlapComponent;

    //*******************************************************************************************************************
    //                                          PROTECTED FUNCTIONS                                                     *
    //*******************************************************************************************************************

    /*It will check if the player is above this weapon using the SphereOverlap so it can tell to the PlayerController
    that there is a weapon available to pick.*/
    void OnPlayerAboveWeapon(const bool bIsAbove);

    /*Wait a few seconds to enable this pickup so the player is not able to take this weapon as soon as it is spawned.*/
    UFUNCTION()
    void EnableOverlapEventsAfterSpawn();
    UFUNCTION()
    void SphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    UFUNCTION()
    void SphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:

    //*******************************************************************************************************************
    //                                          PRIVATE VARIABLES                                                       *
    //*******************************************************************************************************************

    UPROPERTY(EditDefaultsOnly, Category = "Weapon SFX")
    class USoundCue* TakeWeaponSound;

    const float OverlapEnableTimer = 1.0f;

    //*******************************************************************************************************************
    //                                          PRIVATE FUNCTIONS                                                       *
    //*******************************************************************************************************************

    virtual void BeginPlay() override;
};