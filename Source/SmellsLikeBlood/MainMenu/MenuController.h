//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MenuController.generated.h"

UCLASS()
class SMELLSLIKEBLOOD_API AMenuController : public APlayerController
{
    GENERATED_BODY()

public:

    //*******************************************************************************************************************
    //                                          PUBLIC FUNCTIONS                                                        *
    //*******************************************************************************************************************

    //Constructor
    AMenuController();

    /*Creates the Main Menu UI and adds to the viewport.*/
    UFUNCTION(BlueprintImplementableEvent)
    void SetHUDOnScreen();

    /*Add Settings Widget to screen. Called from Options Button*/
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void SetSettingsHUDOnScreen();

    /*Add Settings Widget to screen. Called from Controls Button*/
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void SetControlsHUDOnScreen();

    /*Add Credits Widget to screen. Called from Controls Button*/
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void ShowCreditsOnScreen();

    UFUNCTION(BlueprintCallable)
    void PlayNewGame();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void ChangeToPlayableLevel();

protected:

    //*******************************************************************************************************************
    //                                          PROTECTED FUNCTIONS                                                     *
    //*******************************************************************************************************************

    UPROPERTY(BlueprintReadOnly)
    FName InitialLevelName = "Supermarket_Map";

    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    UFUNCTION(BlueprintCallable)
    void StopPlayerMovement();

    /*Backs to the Main Menu con back button press.*/
    UFUNCTION(BlueprintImplementableEvent)
    void BackToMainMenuHUD();

};