//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#include "Walk_PlayerCharacter.h"

#include "Camera/CameraComponent.h"

#include "Components/CapsuleComponent.h"
#include "Components/PostProcessComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

AWalk_PlayerCharacter::AWalk_PlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->MaxWalkSpeed = 200.0f;

    GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));

    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComponent->SetupAttachment(GetCapsuleComponent());
    SpringArmComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 80.0f));
    SpringArmComponent->TargetArmLength = 110.f;
    SpringArmComponent->SocketOffset    = FVector(0.0f, 30.0f, 0.0f);
    SpringArmComponent->TargetOffset    = FVector(0.0f, 30.0f, 0.0f);
    SpringArmComponent->bUsePawnControlRotation = true;

    PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
    PlayerCamera->SetupAttachment(SpringArmComponent);

    BlackFadeCameraPP = CreateDefaultSubobject<UPostProcessComponent>(TEXT("BlackFade"));
    DefaultCameraPP   = CreateDefaultSubobject<UPostProcessComponent>(TEXT("DefaultCamera"));

    DefaultCameraPP->BlendWeight   = 1.0f;
    BlackFadeCameraPP->BlendWeight = 1.0f;
    DefaultCameraPP->Priority      = 0;
    BlackFadeCameraPP->Priority    = 1;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AWalk_PlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    FadeOutCamera();
    UGameplayStatics::PlaySound2D(GetWorld(), IntroSoundVoice);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AWalk_PlayerCharacter::PlayerWalkForward(float ForwardInput)
{
    if (ForwardInput != 0.0f)
    {
        const FVector& CameraForward = PlayerCamera->GetForwardVector();
        AddMovementInput(CameraForward, ForwardInput);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AWalk_PlayerCharacter::PlayerWalkRight(float RightInput)
{
    if (RightInput != 0.0f)
    {
        const FVector& CameraRight = PlayerCamera->GetRightVector();
        AddMovementInput(CameraRight, RightInput);
    }
}