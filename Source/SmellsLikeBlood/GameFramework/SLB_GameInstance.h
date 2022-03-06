//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"

#include "Engine/GameInstance.h"
#include "SLB_GameInstance.generated.h"

UCLASS()
class SMELLSLIKEBLOOD_API USLB_GameInstance : public UGameInstance
{
    GENERATED_BODY()

public:

    //*******************************************************************************************************************
    //                                          PUBLIC FUNCTIONS & VARIABLES                                            *
    //*******************************************************************************************************************

    void OnCreateAndSaveInitialData();

    /*Forces a new game.*/
    UFUNCTION(BlueprintCallable)
    void StartNewGame();

    UPROPERTY(BlueprintReadWrite)
    class USaveGameMaster* OriginalSaveFile;

protected:

    //*******************************************************************************************************************
    //                                          PROTECTED FUNCTIONS                                                     *
    //*******************************************************************************************************************

    virtual void Init() override;

    UFUNCTION(BlueprintImplementableEvent)
    void AsyncSaveAllData();

private:

    //*******************************************************************************************************************
    //                                          PRIVATE VARIABLES                                                       *
    //*******************************************************************************************************************

    UPROPERTY(EditDefaultsOnly, Category = "Save Data Info")
    TSubclassOf<class USaveGameMaster> SaveMasterData;
};