//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SmellsLikeBlood/SmellsLikeBlood.h"
#include "TPS_PlayerController.generated.h"

UCLASS()
class SMELLSLIKEBLOOD_API ATPS_PlayerController : public APlayerController
{
    GENERATED_BODY()

    friend class ATPS_PlayerCharacter;

public:

    //*******************************************************************************************************************
    //                                         CONSTRUCTOR && PUBLIC VARIABLES                                          *
    //*******************************************************************************************************************

    //Constructor
    ATPS_PlayerController();

    float _MoveForwardInput;
    float _MoveRightInput;
    float HorizontalLookSensitivity;

    UPROPERTY(EditDefaultsOnly, Category = "Look Axis")
    float NotAimingHorizontalAxis;
    UPROPERTY(EditDefaultsOnly, Category = "Look Axis")
    float AimingHorizontalAxis;
    UPROPERTY(EditDefaultsOnly, Category = "Look Axis")
    float VerticalLookSensitivity;
    UPROPERTY(BlueprintReadWrite, Category = "Animation Variables")
    float MoveForward;
    UPROPERTY(BlueprintReadWrite, Category = "Animation Variables")
    float MoveRight;

    bool bInvertLook;

    UPROPERTY(BlueprintReadWrite, Category = "Animation Variables")
    bool _bIsSprinting;
    UPROPERTY(BlueprintReadWrite, Category = "Animation Variables")
    bool bIsAiming;

    //*******************************************************************************************************************
    //                                          PUBLIC FUNCTIONS                                                        *
    //*******************************************************************************************************************

    /*Tells the player if we are or not are sprinting.*/
    UFUNCTION(BlueprintCallable)
    void SetIsSprinting(bool bIsSprinting);
    /*Called from PickupWeapon to know if we have a weapon available to pick from the floor.*/
    UFUNCTION(BlueprintCallable)
    void SetWeaponAvailableToPick(const bool& bIsAvailable, const EAllWeaponTypes& AvailableWeapon, APickupWeapon* WeaponAsset_Ref);
    /*Shows/Hides the UI. Useful to cinematic or radio call situation.*/
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void ShowOrHideAllGameplayUI(bool bHideTheUI);
    /*Handles all the UI/Pause on game.*/
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void GameOnPause(bool bOnPause);
    /*Handles all the radio calls in the whole game.*/
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void GameOnDialogueMode(bool bEnableDialogue, const int32 DialogeLevelValue, const int32 DialogueIndexValue);
    /*Called from pause menu to reload current level.*/
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void ReloadLevelFromPause(bool bGoToMainMenu);
    UFUNCTION(BlueprintImplementableEvent)
    void StrippersDialogueWarning();

protected:

    //*******************************************************************************************************************
    //                                          PROTECTED VARIABLES                                                     *
    //*******************************************************************************************************************

    class APickupWeapon* CurrentWeaponOnFloor;

    UPROPERTY(BlueprintReadOnly)
    class ATPS_PlayerCharacter* PlayerCharacter;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
    TSubclassOf<UUserWidget> GameplayWidgetClass;

    UPROPERTY(BlueprintReadWrite, Category = "Widgets")
    UUserWidget* GameplayWidget;

    EAllWeaponTypes CurrentWeaponAvailableToPick;

    bool bDisablePauseAndLookOnDeadPlayer;

    //*******************************************************************************************************************
    //                                          PROTECTED FUNCTIONS                                                     *
    //*******************************************************************************************************************

    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    void PlayerIsDead();

    /*Saves the forward input value. Used to know where to move on cover depending on camera position.*/
    void SaveForwardCurrentInput(float CurrentMoveForwardInput);
    /*Saves the right input value. Used to know where to move on cover depending on camera position.*/
    void SaveRightCurrentInput(float CurrentMoveRightInput);
    /*Calculates where to move depending on camera position.*/
    void CalculateMovement(float MoveForwardInput, float MoveRightInput);

    /*Vertical camera look input.*/
    void LookUp(float Val);
    /*Horizontal camera look input.*/
    void LookRight(float Val);
    /*Sprint input.*/
    void PlayerSprint();
    /*Cover input.*/
    void TakeCover();
    /*Press Aim input.*/
    void PlayerStartAim();
    /*Release Aim input.*/
    void PlayerStopAim();
    /*Press Shot input.*/
    void PlayerStartFire();
    /*Release Shot input.*/
    void PlayerStopFire();
    /*Tells the player to take a new weapon from the floor if it is available.*/
    void PlayerTakesANewWeapon();
    /*Press right arrow input.*/
    void PlayerSwitchToRifle();
    /*Press up arrow input.*/
    void PlayerSwitchToShotgun();
    /*Press left arrow input.*/
    void PlayerSwitchToSniper();
    /*Press Cool down input.*/
    void PlayerForceCoolDown();
    /*Release Cool down input.*/
    void PlayerStopForceCoolDown();
    /*Press physic attack input.*/
    void PlayerMeleeAttack();
    /*Press Slow time input.*/
    void PlayerSlowBullet();
    /*Executes joystick vibration depending of event type.*/
    void PlayRumbleFeedBack(const ERumbleFeedBack& RumbleType);
    /*Pause input.*/
    void OnPauseInput();

    //*******************************************************************************************************************
    //                                          PROTECTED UI BlueprintEvents(functions)                                 *
    //*******************************************************************************************************************

    /*Creates the Gameplay UI and adds to the viewport.*/
    UFUNCTION(BlueprintImplementableEvent)
    void SetHUDOnScreen();

    /*Turns on/off the cross-hair visibility.*/
    UFUNCTION(BlueprintImplementableEvent)
    void SetCrossHairVisibility(const bool bIsCrosshairVisible);

    /*Changes cross-hair color to red if we are aiming an enemy.*/
    UFUNCTION(BlueprintImplementableEvent)
    void SetCrossHairColor(const bool bIsEnemyOnTarget);

    /*Updates health bar.*/
    UFUNCTION(BlueprintImplementableEvent)
    void SetLifeHUDValue(const float LifePercent);

    /*Updates heat bar.*/
    UFUNCTION(BlueprintImplementableEvent)
    void SetWeaponHeatHUDValue(const float HeatAmount);

    /*Updates slow time bar.*/
    UFUNCTION(BlueprintImplementableEvent)
    void SetSlowTimeHUDValue(const float SlowTimeValue);

    /*Updates heat text.*/
    UFUNCTION(BlueprintImplementableEvent)
    void SetWeaponIsOverHeatedHUDValue(const bool bOverHeatedWeapon);

    /*Updates current weapon equipped.*/
    UFUNCTION(BlueprintImplementableEvent)
    void SetCurrentWeaponHUD(const int32& CurrentWeaponIndex);

    /*Updates type of weapons equipped.. Like legendary or normal in the slows.*/
    UFUNCTION(BlueprintImplementableEvent)
    void SetWeaponSlotsHUD();

    /*Show player weapons when switching.*/
    UFUNCTION(BlueprintImplementableEvent)
    void ShowWeaponSwitchHUD();

    /*Show which weapon is available to pick from the floor.*/
    UFUNCTION(BlueprintImplementableEvent)
    void ShowWeaponAvailableToPickHUD(const int32& WeaponIndex);

    /*Plays hit mark animation when the player kills an enemy.*/
    UFUNCTION(BlueprintImplementableEvent)
    void SetHitmarkHUD();

    /*Disable inputs and set Death UI*/
    UFUNCTION(BlueprintImplementableEvent)
    void PlayerIsDeadUISet();

    /*Calls the Stop Aim and Stop Fire function so the player doesn't keep firing after radio speak.*/
    UFUNCTION(BlueprintCallable)
    void SetPlayerToDialogueMode();
};