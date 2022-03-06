//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialManager.generated.h"

UENUM(BlueprintType)
enum class EPlayerTutorialState : uint8
{
    OnCoverState,
    OnShootState,
    OnMeleeState,
    OnSlowTime,
    CooldownReminder
};

UCLASS()
class SMELLSLIKEBLOOD_API ATutorialManager : public AActor
{
    GENERATED_BODY()
    
public:

    //*******************************************************************************************************************
    //                                         CONSTRUCTOR && PUBLIC VARIABLES                                          *
    //*******************************************************************************************************************

    //Constructor
    ATutorialManager();

    bool bTargetsDestroyed;
    bool bMeleeRobotsDestroyed;
    bool bSlowTimeRobotsDestroyed;

    //*******************************************************************************************************************
    //                                          PUBLIC FUNCTIONS                                                        *
    //*******************************************************************************************************************

    /*Checks if all initial robots has been destroyed.*/
    void TutorialRobotDead();
    /*Checks if all physic attack robots has been destroyed.*/
    void TutorialMeleeRobotDead();
    /*Checks if all slow time robots has been destroyed.*/
    void TutorialSlowTimeRobotDead();

protected:

    //*******************************************************************************************************************
    //                                          PROTECTED VARIABLES                                                     *
    //*******************************************************************************************************************

    class ATPS_PlayerCharacter* PlayerCharacterRef;

    UPROPERTY(EditAnywhere, Category = "Level completed trigger")
    class ATriggerBox* EndLevelTriggerBox;

    UPROPERTY(EditAnywhere, Category = "Dialogues")
    TArray<class ADialogueMaster*> AllDialogues;
    UPROPERTY(EditAnywhere, Category = "Tutorial Robots")
    TArray<class ATutorialRobot*> TutorialRobots;
    UPROPERTY(EditAnywhere, Category = "Tutorial Robots")
    TArray<class ATutorialRobot*> TutorialMeleeRobots;
    UPROPERTY(EditAnywhere, Category = "Tutorial Robots")
    TArray<class ATutorialRobot*> SlowTimerRobots;

    int32 AmountOfRobots;

    EPlayerTutorialState CurrentTutorialState;

    //*******************************************************************************************************************
    //                                          PROTECTED FUNCTIONS                                                     *
    //*******************************************************************************************************************

    virtual void BeginPlay() override;
    virtual void Tick( float DeltaTime) override;

    /*Checks the current tutorial state.*/
    void CheckCurrentState();
    /*If cover tutorial was completed, it will call OnCoverCompletedSuccess.*/
    void OnCoverCompleted();
    /*If shoot tutorial was completed, it will call OnShootCompletedSuccess.*/
    void OnShootCompleted();
    /*If physic attack tutorial was completed, it will call OnMeleeCompletedSuccess.*/
    void OnMeleeCompleted();
    /*If slow time tutorial was completed, it will call OnSlowTimeCompletedSuccess.*/
    void OnSlowTimeCompleted();

    /*Calls the speak manager, spawn robots and change to Shoot State.*/
    UFUNCTION()
    void OnCoverCompletedSuccess();
    /*Calls the speak manager, Turn Off the robots and change to attack State.*/
    UFUNCTION()
    void OnShootCompletedSuccess();
    /*Calls the speak manager, Turn Off the attack type robots and change to slow time State.*/
    UFUNCTION()
    void OnMeleeCompletedSuccess();
    /*Calls the speak manager, Enable the level trigger to advance to the next level and destroy this object.*/
    UFUNCTION()
    void OnSlowTimeCompletedSuccess();

private:

    //*******************************************************************************************************************
    //                                          PRIVATE VARIABLES                                                       *
    //*******************************************************************************************************************

    bool bCoverCompleted;
    bool bShooterTutorialCompleted;
    bool bMeleeCompleted;
    bool bSlowTimeCompleted;
    bool bSlowTimeWasActivated;

    //*******************************************************************************************************************
    //                                          PRIVATE FUNCTIONS                                                       *
    //*******************************************************************************************************************

    void TurnOnOffTheRobot(const bool& bTurnOn, const bool& bShooterRobots);
};