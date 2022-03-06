//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CoverInterface.generated.h"

UINTERFACE(MinimalAPI)
class UCoverInterface : public UInterface
{
    GENERATED_BODY()
};

class SMELLSLIKEBLOOD_API ICoverInterface
{
    GENERATED_BODY()

public:

    //*******************************************************************************************************************
    //                                          PUBLIC FUNCTIONS                                                        *
    //*******************************************************************************************************************

    /*Called from Player and CoverObject to set if there is cover available.*/
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void IsCoverAvailable(bool bIsCoverAvailable, FRotator CoverRotation);
    /*Called from Player and CoverObject to set if this is the end of the right or left side of this cover object.*/
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void EndOfCover(bool bEndOfCoverLeft, bool bEndOfCoverRight);
};