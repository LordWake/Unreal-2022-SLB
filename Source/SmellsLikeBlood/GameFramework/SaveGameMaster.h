//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameMaster.generated.h"

#define SAVE_WEAPON_NONE      "None"
#define SAVE_WEAPON_NORMAL    "Normal"
#define SAVE_WEAPON_LEGENDARY "Legendary"

UCLASS()
class SMELLSLIKEBLOOD_API USaveGameMaster : public USaveGame
{
    GENERATED_BODY()

public:

    //*******************************************************************************************************************
    //                                          PLAYER WEAPONS INFO                                                     *
    //*******************************************************************************************************************

    FName AssaultRifleType = SAVE_WEAPON_NORMAL;
    FName ShotgunType      = SAVE_WEAPON_NONE;
    FName SniperType       = SAVE_WEAPON_NONE;

    //*******************************************************************************************************************
    //                                          SAVE DATA VARIABLES                                                     *
    //*******************************************************************************************************************

    UPROPERTY(BlueprintReadOnly, Category = "Save Game Data")
    FName MasterSaveSlotName  = "Slot_01";
    UPROPERTY(BlueprintReadOnly, Category = "Save Game Data")
    int32 MasterSaveSlowIndex = 0;

    //*******************************************************************************************************************
    //                                          LEVELS DATA                                                             *
    //*******************************************************************************************************************

    UPROPERTY(BlueprintReadWrite, Category = "Saved Values")
    FName LevelToLoad;
    UPROPERTY(BlueprintReadWrite, Category = "Saved Values")
    bool bDoNotPlayCinematics;

};

#undef SAVE_WEAPON_NONE
#undef SAVE_WEAPON_NORMAL
#undef SAVE_WEAPON_LEGENDARY