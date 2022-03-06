//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "TutorialRobot.generated.h"

UCLASS()
class SMELLSLIKEBLOOD_API ATutorialRobot : public AActor
{
    GENERATED_BODY()

public:

    //*******************************************************************************************************************
    //                                         CONSTRUCTOR && PUBLIC VARIABLES                                          *
    //*******************************************************************************************************************

    //Constructor
    ATutorialRobot();

    bool bRobotNotTakingDamage;

protected:

    //*******************************************************************************************************************
    //                                          PROTECTED VARIABLES                                                     *
    //*******************************************************************************************************************

    UPROPERTY(EditAnywhere, Category = "Robot Data")
    class ATutorialManager* TutorialManager;

    UPROPERTY(EditAnywhere, Category = "Robot Data")
    int32 MaxHealth;

    bool bHasBeenDestroyed;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Robot Data")
    bool bMeleeRobot;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Robot Data")
    bool bSlowTimeRobot;

    UPROPERTY(EditDefaultsOnly, Category = "VFX")
    UParticleSystem* ExplosionVFX;

    UPROPERTY(EditDefaultsOnly, Category = "SFX")
    class USoundCue* DestroyedRobotCue;
    UPROPERTY(EditDefaultsOnly, Category = "SFX")
    class USoundCue* DestroyedMeleeRobotCue;

    //*******************************************************************************************************************
    //                                          PROTECTED FUNCTIONS                                                     *
    //*******************************************************************************************************************

    /*Takes damage and do different things depending of what type of robot is.*/
    UFUNCTION(BlueprintCallable)
    void RobotTakeDamage();
    /*Handles the destructible mesh on robot's death.*/
    UFUNCTION(BlueprintImplementableEvent)
    void RobotIsDead();
};