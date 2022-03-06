//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Walk_PlayerController.generated.h"


UCLASS()
class SMELLSLIKEBLOOD_API AWalk_PlayerController : public APlayerController
{
    GENERATED_BODY()

public:

    //*******************************************************************************************************************
    //                                         PUBLIC FUNCTIONS                                                         *
    //*******************************************************************************************************************

    //Constructor
    AWalk_PlayerController();

    /*Creates Pause Widget Blueprint.*/
    UFUNCTION(BlueprintImplementableEvent)
    void CreateAndSetPauseUI();

    /*Creates dialogue Widget Blueprint.*/
    UFUNCTION(BlueprintImplementableEvent)
    void CreateAndSetDialogueUI();

    /*Handles all the UI/Pause on game.*/
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void GameOnPause(bool bOnPause);

    /*Called from pause menu to reload current level.*/
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void ReloadLevelFromPause(bool bGoToMainMenu);

    /*Handles all the radio calls in the whole game.*/
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void GameOnDialogueMode(bool bEnableDialogue, const int32 DialogeLevelValue, const int32 DialogueIndexValue);

private:

    class AWalk_PlayerCharacter* WalkCharacterRef;

private:

    //*******************************************************************************************************************
    //                                         PRIVATE FUNCTIONS                                                        *
    //*******************************************************************************************************************

    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    void OnPauseInput();

    /* Movement and look methods. */
    void MoveForward(float Val);
    void MoveRight(float Val);
    void LookUp(float PitchValue);
    void LookRight(float YawValue);

};