//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#include "TPS_PlayerController.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "SmellsLikeBlood/CommonObjects/PickupWeapon.h"
#include "SmellsLikeBlood/Player/TPS_PlayerCharacter.h"

ATPS_PlayerController::ATPS_PlayerController()
{}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerController::BeginPlay()
{
    Super::BeginPlay();

    PlayerCharacter              = Cast<ATPS_PlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    CurrentWeaponAvailableToPick = EAllWeaponTypes::None;
    HorizontalLookSensitivity    = NotAimingHorizontalAxis;
    bShowMouseCursor             = false;

    SetHUDOnScreen();
    ShowWeaponAvailableToPickHUD(0);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    //Input Axis.
    InputComponent->BindAxis("MoveForward", this, &ATPS_PlayerController::SaveForwardCurrentInput);
    InputComponent->BindAxis("MoveRight",   this, &ATPS_PlayerController::SaveRightCurrentInput);
    InputComponent->BindAxis("LookUp",      this, &ATPS_PlayerController::LookUp);
    InputComponent->BindAxis("LookRight",   this, &ATPS_PlayerController::LookRight);

    //Input Actions.
    InputComponent->BindAction("Sprint",           IE_Pressed,  this, &ATPS_PlayerController::PlayerSprint);
    InputComponent->BindAction("CoverButton",      IE_Pressed,  this, &ATPS_PlayerController::TakeCover);
    InputComponent->BindAction("TakeWeaponButton", IE_Pressed,  this, &ATPS_PlayerController::PlayerTakesANewWeapon);
    InputComponent->BindAction("SwitchRifle",      IE_Pressed,  this, &ATPS_PlayerController::PlayerSwitchToRifle);
    InputComponent->BindAction("SwitchShotgun",    IE_Pressed,  this, &ATPS_PlayerController::PlayerSwitchToShotgun);
    InputComponent->BindAction("SwitchSniper",     IE_Pressed,  this, &ATPS_PlayerController::PlayerSwitchToSniper);
    InputComponent->BindAction("Aim",              IE_Pressed,  this, &ATPS_PlayerController::PlayerStartAim);
    InputComponent->BindAction("Fire",             IE_Pressed,  this, &ATPS_PlayerController::PlayerStartFire);
    InputComponent->BindAction("CoolDownWeapon",   IE_Pressed,  this, &ATPS_PlayerController::PlayerForceCoolDown);
    InputComponent->BindAction("MeleeAttack",      IE_Pressed,  this, &ATPS_PlayerController::PlayerMeleeAttack);
    InputComponent->BindAction("SlowBullet",       IE_Pressed,  this, &ATPS_PlayerController::PlayerSlowBullet);
    InputComponent->BindAction("PauseGame",        IE_Pressed,  this, &ATPS_PlayerController::OnPauseInput);
    InputComponent->BindAction("Aim",              IE_Released, this, &ATPS_PlayerController::PlayerStopAim);
    InputComponent->BindAction("Fire",             IE_Released, this, &ATPS_PlayerController::PlayerStopFire);
    InputComponent->BindAction("CoolDownWeapon",   IE_Released, this, &ATPS_PlayerController::PlayerStopForceCoolDown);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerController::PlayerIsDead()
{
    bDisablePauseAndLookOnDeadPlayer = true;
    GameOnPause(false);
    PlayerIsDeadUISet();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerController::SetIsSprinting(bool bIsSprinting)
{
    _bIsSprinting = bIsSprinting;
    PlayerCharacter->PlayerIsSprinting(_bIsSprinting);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerController::SetWeaponAvailableToPick(const bool& bIsAvailable, const EAllWeaponTypes& AvailableWeapon, APickupWeapon* WeaponAsset_Ref)
{
    if (bIsAvailable)
    {
        CurrentWeaponAvailableToPick = AvailableWeapon;
        CurrentWeaponOnFloor         = WeaponAsset_Ref;
        ShowWeaponAvailableToPickHUD(static_cast<int32>(AvailableWeapon));
    }
    else
    {
        CurrentWeaponAvailableToPick = EAllWeaponTypes::None;
        CurrentWeaponOnFloor         = nullptr;
        ShowWeaponAvailableToPickHUD(0);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerController::SaveForwardCurrentInput(float CurrentMoveForwardInput)
{
    _MoveForwardInput = CurrentMoveForwardInput;
    CalculateMovement(_MoveForwardInput, _MoveRightInput);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerController::SaveRightCurrentInput(float CurrentMoveRightInput)
{
    _MoveRightInput = CurrentMoveRightInput;
    CalculateMovement(_MoveForwardInput, _MoveRightInput);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerController::CalculateMovement(float MoveForwardInput, float MoveRightInput)
{
    FRotator DeltaResult = UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), 
                           UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorRotation());

    MoveForward = (MoveForwardInput * UKismetMathLibrary::DegCos(DeltaResult.Yaw)) +
                  (MoveForwardInput * UKismetMathLibrary::DegSin(DeltaResult.Yaw));

    MoveRight   = (MoveRightInput * UKismetMathLibrary::DegCos(0.0f - DeltaResult.Yaw)) +
                  (MoveRightInput * UKismetMathLibrary::DegSin(0.0f - DeltaResult.Yaw));
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerController::LookUp(float Val)
{
    if (bDisablePauseAndLookOnDeadPlayer)
    {
        return;
    }

    float TempPitchValue = Val * VerticalLookSensitivity;
    if (bInvertLook)
    {
        TempPitchValue = TempPitchValue * -1.0f;
    }
    AddPitchInput(TempPitchValue);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerController::LookRight(float Val)
{
    if (bDisablePauseAndLookOnDeadPlayer)
    {
        return;
    }

    float TempYawValue = Val * HorizontalLookSensitivity;
    if (bInvertLook)
    {
        TempYawValue = TempYawValue * -1.0f;
    }
    AddYawInput(TempYawValue);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerController::PlayerSprint()
{
    if (MoveForward > 0.0f && !PlayerCharacter->bPlayerIsTakingCover)
    {
        _bIsSprinting = !_bIsSprinting;
        PlayerCharacter->SetCanFire(!_bIsSprinting);
        PlayerCharacter->PlayerIsSprinting(_bIsSprinting);

        if (_bIsSprinting)
        {
            PlayerCharacter->SetFOV(false);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerController::TakeCover()
{
    PlayerCharacter->TakeCover();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerController::PlayerStartAim()
{
    if (!_bIsSprinting)
    {
        HorizontalLookSensitivity = AimingHorizontalAxis;
        bIsAiming                 = true;

        SetCrossHairVisibility(true);
        PlayerCharacter->SetFOV(bIsAiming);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerController::PlayerStopAim()
{
    HorizontalLookSensitivity = NotAimingHorizontalAxis;
    bIsAiming                 = false;

    if (PlayerCharacter)
    {
        PlayerCharacter->SetFOV(bIsAiming);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerController::PlayerStartFire()
{
    if (!_bIsSprinting)
    {
        PlayerCharacter->FireWeapon();

        if (!bIsAiming)
        {
            PlayerCharacter->SetFOV(true);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerController::PlayerStopFire()
{
    if (!bIsAiming && PlayerCharacter)
    {
        PlayerCharacter->SetFOV(false);
    }

    if (PlayerCharacter)
    {
        PlayerCharacter->StopFireWeapon();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerController::PlayerTakesANewWeapon()
{
    if (CurrentWeaponAvailableToPick != EAllWeaponTypes::None && CurrentWeaponOnFloor != nullptr)
    {
        if (PlayerCharacter->TakeNewWeapon(CurrentWeaponAvailableToPick))
        {
            CurrentWeaponOnFloor->OnPlayerTakeWeapon();
            CurrentWeaponAvailableToPick = EAllWeaponTypes::None;
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerController::PlayerSwitchToRifle()
{
    PlayerCharacter->SwitchWeapons(EAllWeaponTypes::AssaultRifleWeapon);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerController::PlayerSwitchToShotgun()
{
    PlayerCharacter->SwitchWeapons(EAllWeaponTypes::ShotgunWeapon);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerController::PlayerSwitchToSniper()
{
    PlayerCharacter->SwitchWeapons(EAllWeaponTypes::SniperRifleWeapon);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerController::PlayerForceCoolDown()
{
    PlayerCharacter->ForceWeaponCoolDown(true);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerController::PlayerStopForceCoolDown()
{
    PlayerCharacter->ForceWeaponCoolDown(false);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerController::PlayerMeleeAttack()
{
    PlayerCharacter->PlayerStartsMeleeAttack();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerController::PlayerSlowBullet()
{
    PlayerCharacter->PlayerSlowTimeBullet();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerController::PlayRumbleFeedBack(const ERumbleFeedBack& RumbleType)
{
    switch (RumbleType)
    {
        default:
        break;

        case ERumbleFeedBack::LowHealthFeedBack:
        PlayDynamicForceFeedback(0.75f, 0.25f, true, true, true, true, EDynamicForceFeedbackAction::Start, 0Ui64);
        break;

        case ERumbleFeedBack::RifleFeedBack:
        PlayDynamicForceFeedback(0.65f, 0.1f, true, true, true, true, EDynamicForceFeedbackAction::Start, 0Ui64);
        break;

        case ERumbleFeedBack::ShotgunFeedBack:
        PlayDynamicForceFeedback(1.0f, 0.5f, true, true, true, true, EDynamicForceFeedbackAction::Start, 0Ui64);
        break;

        case ERumbleFeedBack::SniperFeedBack:
        PlayDynamicForceFeedback(1.0f, 1.0f, true, true, true, true, EDynamicForceFeedbackAction::Start, 0Ui64);
        break;

        case ERumbleFeedBack::MeleeFeedBack:
        PlayDynamicForceFeedback(1.0f, 0.75f, true, true, true, true, EDynamicForceFeedbackAction::Start, 0Ui64);
        break;

        case ERumbleFeedBack::TakeDamageFeedBack:
        PlayDynamicForceFeedback(0.75f, 0.2f, true, true, true, true, EDynamicForceFeedbackAction::Start, 0Ui64);
        break;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerController::OnPauseInput()
{
    if (!bDisablePauseAndLookOnDeadPlayer)
    {
        GameOnPause(true);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATPS_PlayerController::SetPlayerToDialogueMode()
{
    PlayerStopAim();
    PlayerStopFire();
}