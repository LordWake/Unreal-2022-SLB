//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamageInterface.generated.h"

UINTERFACE(MinimalAPI)
class UDamageInterface : public UInterface
{
    GENERATED_BODY()
};

class SMELLSLIKEBLOOD_API IDamageInterface
{
    GENERATED_BODY()

public:

    //*******************************************************************************************************************
    //                                          PUBLIC FUNCTIONS                                                        *
    //*******************************************************************************************************************

    /*Called from Player, Enemies and Projectiles to know if we can take damage.*/
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void TakeDamage(float DamageMin, float DamageMax, float CriticChance, FName HitBoneName, AActor* DamageInstigator);
};