//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#include "TutorialManager.h"

#include "Engine/TriggerBox.h"

#include "Kismet/GameplayStatics.h"

#include "SmellsLikeBlood/Dialogue/DialogueMaster.h"
#include "SmellsLikeBlood/Player/TPS_PlayerCharacter.h"
#include "SmellsLikeBlood/Tutorial/TutorialRobot.h"

ATutorialManager::ATutorialManager()
{
    PrimaryActorTick.bCanEverTick = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATutorialManager::TutorialRobotDead()
{
    AmountOfRobots--;
    if (AmountOfRobots == TutorialRobots.Num() - 1)
    {
        //Activate this just as a simple reminder after kill one robot.
        AllDialogues[(int32)EPlayerTutorialState::CooldownReminder]->ActivateDialogue();
    }

    if (AmountOfRobots <= 0)
    {
        bTargetsDestroyed = true; 
        AmountOfRobots    = TutorialMeleeRobots.Num();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATutorialManager::TutorialMeleeRobotDead()
{
    AmountOfRobots--;
    if (AmountOfRobots <= 0)
    {
        bMeleeRobotsDestroyed = true; 
        AmountOfRobots        = SlowTimerRobots.Num();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATutorialManager::TutorialSlowTimeRobotDead()
{
    AmountOfRobots--;
    if (AmountOfRobots <= 0)
    {
        bSlowTimeRobotsDestroyed = true; 
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATutorialManager::BeginPlay()
{
    Super::BeginPlay();

    CurrentTutorialState = EPlayerTutorialState::OnCoverState;
    PlayerCharacterRef   = Cast<ATPS_PlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    AmountOfRobots       = TutorialRobots.Num();

    TurnOnOffTheRobot(false, true);
    TurnOnOffTheRobot(false, false);

    EndLevelTriggerBox->SetActorEnableCollision(false);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATutorialManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    CheckCurrentState();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATutorialManager::CheckCurrentState()
{
    switch (CurrentTutorialState)
    {
        case EPlayerTutorialState::OnCoverState: OnCoverCompleted();    break;
        case EPlayerTutorialState::OnShootState: OnShootCompleted();    break;
        case EPlayerTutorialState::OnMeleeState: OnMeleeCompleted();    break;
        case EPlayerTutorialState::OnSlowTime:   OnSlowTimeCompleted(); break;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATutorialManager::OnCoverCompleted()
{
    if(PlayerCharacterRef->bPlayerIsTakingCover && !bCoverCompleted)
    {
        bCoverCompleted = true;

        FLatentActionInfo LatentInfo;
        LatentInfo.CallbackTarget    = this;
        LatentInfo.ExecutionFunction = FName("OnCoverCompletedSuccess");
        LatentInfo.Linkage           = 0;
        LatentInfo.UUID              = 0;

        UKismetSystemLibrary::Delay(GetWorld(), 1.0f, LatentInfo);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATutorialManager::OnCoverCompletedSuccess()
{
    AllDialogues[static_cast<int32>(CurrentTutorialState)]->ActivateDialogue();
    TurnOnOffTheRobot(true, true);
    CurrentTutorialState = EPlayerTutorialState::OnShootState;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATutorialManager::OnShootCompleted()
{
    if (bTargetsDestroyed && !bShooterTutorialCompleted)
    {
        bShooterTutorialCompleted = true;

        FLatentActionInfo LatentInfo;
        LatentInfo.CallbackTarget    = this;
        LatentInfo.ExecutionFunction = FName("OnShootCompletedSuccess");
        LatentInfo.Linkage           = 0;
        LatentInfo.UUID              = 1;

        UKismetSystemLibrary::Delay(GetWorld(), 1.0f, LatentInfo);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATutorialManager::OnShootCompletedSuccess()
{
    AllDialogues[static_cast<int32>(CurrentTutorialState)]->ActivateDialogue();
    TurnOnOffTheRobot(true, false);
    CurrentTutorialState = EPlayerTutorialState::OnMeleeState;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATutorialManager::TurnOnOffTheRobot(const bool& bTurnOn, const bool& bShooterRobots)
{
    if (bShooterRobots)
    {
        for (ATutorialRobot* ThisRobot : TutorialRobots)
        {
            ThisRobot->SetActorHiddenInGame(!bTurnOn);
            ThisRobot->SetActorEnableCollision(bTurnOn);
        }
    }
    else
    {
        for (ATutorialRobot* ThisRobot : TutorialMeleeRobots)
        {
            ThisRobot->SetActorHiddenInGame(!bTurnOn);
            ThisRobot->SetActorEnableCollision(bTurnOn);
        }
    }

    for (ATutorialRobot* ThisRobot : SlowTimerRobots)
    {
        ThisRobot->SetActorHiddenInGame(true);
        ThisRobot->SetActorEnableCollision(false);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATutorialManager::OnMeleeCompleted()
{
    if (bMeleeRobotsDestroyed && !bMeleeCompleted)
    {
        bMeleeCompleted = true;

        FLatentActionInfo LatentInfo;
        LatentInfo.CallbackTarget    = this;
        LatentInfo.ExecutionFunction = FName("OnMeleeCompletedSuccess");
        LatentInfo.Linkage           = 0;
        LatentInfo.UUID              = 2;

        UKismetSystemLibrary::Delay(GetWorld(), 1.0f, LatentInfo);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATutorialManager::OnMeleeCompletedSuccess()
{
    AllDialogues[static_cast<int32>(CurrentTutorialState)]->ActivateDialogue();
    CurrentTutorialState = EPlayerTutorialState::OnSlowTime;

    for (ATutorialRobot* ThisRobot : SlowTimerRobots)
    {
        ThisRobot->SetActorHiddenInGame(false);
        ThisRobot->SetActorEnableCollision(true);
        ThisRobot->bRobotNotTakingDamage = true;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATutorialManager::OnSlowTimeCompleted()
{
    //If for some reason the player has no SlowTimer available, we add some.
    if (PlayerCharacterRef->CurrentSlowTimer <= 10.0f)
    {
        PlayerCharacterRef->EnemyKilledByPlayer();
    }

    //Once the player has activate his slow time, we can start making damage to the robots.
    if (PlayerCharacterRef->bIsSlowTimeActivated && !bSlowTimeWasActivated)
    {
        bSlowTimeWasActivated = true;

        for (ATutorialRobot* ThisRobot : SlowTimerRobots)
        {
            ThisRobot->bRobotNotTakingDamage = false; 
        }
    }

    if (bSlowTimeRobotsDestroyed && !bSlowTimeCompleted)
    {
        bSlowTimeCompleted = true;

        FLatentActionInfo LatentInfo;
        LatentInfo.CallbackTarget    = this;
        LatentInfo.ExecutionFunction = FName("OnSlowTimeCompletedSuccess");
        LatentInfo.Linkage           = 0;
        LatentInfo.UUID              = 3;

        UKismetSystemLibrary::Delay(GetWorld(), 1.0f, LatentInfo);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATutorialManager::OnSlowTimeCompletedSuccess()
{
    AllDialogues[(int32)CurrentTutorialState]->ActivateDialogue();
    EndLevelTriggerBox->SetActorEnableCollision(true);
    Destroy(); //Tutorial Completed.
}