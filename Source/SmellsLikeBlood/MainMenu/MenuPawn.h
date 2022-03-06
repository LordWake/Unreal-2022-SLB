//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MenuPawn.generated.h"

UCLASS()
class SMELLSLIKEBLOOD_API AMenuPawn : public APawn
{
    GENERATED_BODY()

    friend class AMenuController;

    //*******************************************************************************************************************
    //                                          COMPONENTS                                                              *
    //*******************************************************************************************************************

    UPROPERTY(VisibleAnywhere, Category = Components, meta = (AllowPrivateAccess = "true"))
    class USkeletalMeshComponent* MenuPlayerMesh;
    UPROPERTY(VisibleAnywhere, Category = Components, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* SpringArmComp;

    class UCameraComponent* CameraComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Effects", meta = (AllowPrivateAccess = "true"))
    class UPostProcessComponent* DefaultMenuCameraPP;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Effects", meta = (AllowPrivateAccess = "true"))
    class UPostProcessComponent* InitializeScenePP;

public:

    //Constructor
    AMenuPawn();

protected:

    //*******************************************************************************************************************
    //                                          PROTECTED FUNCTIONS                                                     *
    //*******************************************************************************************************************

    UFUNCTION(BlueprintImplementableEvent)
    void FadeOutCamera();

private:

    //*******************************************************************************************************************
    //                                          PRIVATE VARIABLES                                                       *
    //*******************************************************************************************************************

    class AMenuController* MenuController;

    FTimerHandle   EnableUI_TimerHandle;
    FTimerDelegate EnableUI_TimerDelegate;
    FTimerHandle   MovePlayer_TimerHandle;
    FTimerDelegate MovePlayer_TimerDelegate;

    //*******************************************************************************************************************
    //                                          PRIVATE FUNCTIONS                                                       *
    //*******************************************************************************************************************

    /*Calls FadeOutCamera zoom time line.*/
    void EnableCameraView();

    /*Stops All Timers so the game won't crash on level change.*/
    void StopAllMovement();
};