//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#include "SLB_GameInstance.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGameMaster.h"
#include "Sound/SoundBase.h"

#define TUTORIAL_LEVEL_NAME "Supermarket_Map"

//------------------------------------------------------------------------------------------------------------------------------------------
void USLB_GameInstance::OnCreateAndSaveInitialData()
{
    if (OriginalSaveFile == nullptr)
    {
        OriginalSaveFile = CastChecked<USaveGameMaster>(UGameplayStatics::CreateSaveGameObject(SaveMasterData));
    }

    if (OriginalSaveFile->LevelToLoad == "")
    {
       OriginalSaveFile->LevelToLoad = TUTORIAL_LEVEL_NAME;
    }

    if (OriginalSaveFile->bDoNotPlayCinematics)
    {
        OriginalSaveFile->bDoNotPlayCinematics = false;
    }

    AsyncSaveAllData();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void USLB_GameInstance::StartNewGame()
{
    if (OriginalSaveFile)
    {
        OriginalSaveFile->LevelToLoad = TUTORIAL_LEVEL_NAME;
        OriginalSaveFile->bDoNotPlayCinematics = false;
    }

    AsyncSaveAllData();
}

//------------------------------------------------------------------------------------------------------------------------------------------
void USLB_GameInstance::Init()
{
    Super::Init();
    OnCreateAndSaveInitialData();
}

#undef TUTORIAL_LEVEL_NAME