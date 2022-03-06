//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#include "DialogueMaster.h"
#include "Kismet/GameplayStatics.h"
#include "SmellsLikeBlood/Player/TPS_PlayerController.h"

ADialogueMaster::ADialogueMaster()
{
    PrimaryActorTick.bCanEverTick = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ADialogueMaster::BeginPlay()
{
    Super::BeginPlay();
    PlayerControllerRef = Cast<ATPS_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ADialogueMaster::ActivateDialogue()
{
    PlayerControllerRef->GameOnDialogueMode(true, LevelDialogue, DialogueIndex);
    Destroy();
}