//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#include "MenuPawn.h"

#include "Camera/CameraComponent.h"

#include "Components/PostProcessComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "GameFramework/SpringArmComponent.h"

#include "MenuController.h"

#define PLAYER_ROTATION_SPEED 1.0f
#define PLAYER_ROTATION_RATE  0.02f
#define ENABLE_UI_TIMER       0.5f

//------------------------------------------------------------------------------------------------------------------------------------------
AMenuPawn::AMenuPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    MenuPlayerMesh = CreateDefaultSubobject<USkeletalMeshComponent>("MenuPlayerMesh");
    SetRootComponent(MenuPlayerMesh);

    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
    SpringArmComp->SetupAttachment(MenuPlayerMesh);
    SpringArmComp->SetRelativeLocation(FVector(0.0f, -5.0f, 160.0f));
    SpringArmComp->SetRelativeRotation(FRotator(0.0f, -110.0f, 0.0f));
    SpringArmComp->TargetArmLength = 100.0f;

    CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
    CameraComp->SetupAttachment(SpringArmComp);

    DefaultMenuCameraPP = CreateDefaultSubobject<UPostProcessComponent>(TEXT("DefaultCameraPP"));
    InitializeScenePP   = CreateDefaultSubobject<UPostProcessComponent>(TEXT("InitializeScenePP"));
    DefaultMenuCameraPP->BlendWeight = 1.0f;
    DefaultMenuCameraPP->Priority    = 0;
    InitializeScenePP->BlendWeight   = 1.0f;
    InitializeScenePP->Priority      = 1;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AMenuPawn::EnableCameraView()
{
    FadeOutCamera();

    #pragma region Lambdass Declaration

    MovePlayer_TimerDelegate.BindLambda
    ([&]()
        {
            const FRotator& Rotation_Rot = FRotator(0.0f, PLAYER_ROTATION_SPEED, 0.0f);
            AddActorWorldRotation(Rotation_Rot, false, false);
        }
    );


    EnableUI_TimerDelegate.BindLambda
    ([&]()
        {
            if (MenuController)
            {
                MenuController->SetHUDOnScreen();
                GetWorldTimerManager().ClearTimer(EnableUI_TimerHandle);
            }
        }
    );

    #pragma endregion

    GetWorldTimerManager().SetTimer(MovePlayer_TimerHandle, MovePlayer_TimerDelegate, PLAYER_ROTATION_RATE, true, ENABLE_UI_TIMER);
    GetWorldTimerManager().SetTimer(EnableUI_TimerHandle,   EnableUI_TimerDelegate,   ENABLE_UI_TIMER, false);
}

void AMenuPawn::StopAllMovement()
{
    GetWorldTimerManager().ClearTimer(MovePlayer_TimerHandle);
}

#undef CAMERA_ROTATION_SPEED
#undef CAMERA_BLACK_FADE_OUT