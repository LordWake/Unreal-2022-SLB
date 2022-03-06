//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#include "Walk_PlayerController.h"

#include "Kismet/GameplayStatics.h"
#include "Walk_PlayerCharacter.h"

AWalk_PlayerController::AWalk_PlayerController()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AWalk_PlayerController::BeginPlay()
{
    Super::BeginPlay();
    WalkCharacterRef = Cast<AWalk_PlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    bShowMouseCursor = false;
    CreateAndSetPauseUI();
    CreateAndSetDialogueUI();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AWalk_PlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    //Input Axis.
    InputComponent->BindAxis("MoveForward", this, &AWalk_PlayerController::MoveForward);
    InputComponent->BindAxis("MoveRight",   this, &AWalk_PlayerController::MoveRight);
    InputComponent->BindAxis("LookUp",      this, &AWalk_PlayerController::LookUp);
    InputComponent->BindAxis("LookRight",   this, &AWalk_PlayerController::LookRight);

    //Input Actions.
    InputComponent->BindAction("PauseGame", IE_Pressed, this, &AWalk_PlayerController::OnPauseInput);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AWalk_PlayerController::OnPauseInput()
{
    GameOnPause(true);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AWalk_PlayerController::MoveForward(float Val)
{
    WalkCharacterRef->PlayerWalkForward(Val);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AWalk_PlayerController::MoveRight(float Val)
{
    WalkCharacterRef->PlayerWalkRight(Val);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AWalk_PlayerController::LookUp(float PitchValue)
{
    float FinalPitch = PitchValue * 0.5f;
    AddPitchInput(FinalPitch);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AWalk_PlayerController::LookRight(float YawValue)
{
    float FinalYaw = YawValue * 0.5f;
    AddYawInput(FinalYaw);
}
