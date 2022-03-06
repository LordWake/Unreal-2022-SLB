//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DialogueMaster.generated.h"

UCLASS()
class SMELLSLIKEBLOOD_API ADialogueMaster : public AActor
{
    GENERATED_BODY()

public:

    //*******************************************************************************************************************
    //                                          CONSTRUCTOR	&& PUBLIC FUNCTIONS                                         *
    //*******************************************************************************************************************

    //Constructor
    ADialogueMaster();

    UFUNCTION(BlueprintCallable)
    void ActivateDialogue();

protected:

    //*******************************************************************************************************************
    //                                          PROTECTED VARIABLES                                                     *
    //*******************************************************************************************************************

    class ATPS_PlayerController* PlayerControllerRef;

    /*0 = TUTORIAL, 1 = WAREHOUSE, 2 = SUBWAY, 3 = NIGHT CLUB, 4 = HANGAR, 5 = DESERT.*/
    UPROPERTY(EditAnywhere, Category = "Dialogue Data")
    int32 LevelDialogue;
    UPROPERTY(EditAnywhere, Category = "Dialogue Data")
    int32 DialogueIndex;
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dialogue Data")
    bool bCalledFromOtherActor;

    //*******************************************************************************************************************
    //                                          PROTECTED FUNCTIONS                                                     *
    //*******************************************************************************************************************

    virtual void BeginPlay() override;
};