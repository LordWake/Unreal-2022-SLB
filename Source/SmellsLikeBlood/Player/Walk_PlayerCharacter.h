//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Walk_PlayerCharacter.generated.h"

UCLASS()
class SMELLSLIKEBLOOD_API AWalk_PlayerCharacter : public ACharacter
{
    GENERATED_BODY()

    friend class AWalk_PlayerController;

    //*******************************************************************************************************************
    //                                          COMPONENTS                                                              *
    //*******************************************************************************************************************

    UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* SpringArmComponent;
    class UCameraComponent* PlayerCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Effects", meta = (AllowPrivateAccess = "true"))
    class UPostProcessComponent* BlackFadeCameraPP;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Effects", meta = (AllowPrivateAccess = "true"))
    class UPostProcessComponent* DefaultCameraPP;

public:

    //*******************************************************************************************************************
    //                                          PUBLIC FUNCTIONS                                                        *
    //*******************************************************************************************************************

    //Constructor
    AWalk_PlayerCharacter();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void FadeInCamera();

protected:

    //*******************************************************************************************************************
    //                                          PROTECTED FUNCTIONS                                                     *
    //*******************************************************************************************************************

    UPROPERTY(EditDefaultsOnly, Category = "SFX")
    class USoundCue* IntroSoundVoice;

    UFUNCTION(BlueprintImplementableEvent)
    void FadeOutCamera();

private:

    virtual void BeginPlay() override;

    void PlayerWalkForward(float ForwardInput);
    void PlayerWalkRight(float RightInput);

};