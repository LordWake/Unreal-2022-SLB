//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#include "MenuController.h"

#include "Kismet/GameplayStatics.h"
#include "MenuPawn.h"

#include "SmellsLikeBlood/GameFramework/SLB_GameInstance.h"

AMenuController::AMenuController()
{}

//------------------------------------------------------------------------------------------------------------------------------------------
void AMenuController::PlayNewGame()
{
    if (USLB_GameInstance* GameInstance = CastChecked<USLB_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
    {
        GameInstance->StartNewGame();
        ChangeToPlayableLevel();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AMenuController::BeginPlay()
{
    Super::BeginPlay();

    if (AMenuPawn* MenuPawn = CastChecked<AMenuPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
    {
        MenuPawn->MenuController = this;
        MenuPawn->EnableCameraView();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AMenuController::SetupInputComponent()
{
    Super::SetupInputComponent();
    InputComponent->BindAction("BackUI", IE_Pressed, this, &AMenuController::BackToMainMenuHUD);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AMenuController::StopPlayerMovement()
{
    if (AMenuPawn* MenuPawn = CastChecked<AMenuPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
    {
        MenuPawn->MenuController = this;
        MenuPawn->StopAllMovement();
    }
}