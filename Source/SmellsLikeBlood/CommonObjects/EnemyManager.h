//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyManager.generated.h"

UCLASS()
class SMELLSLIKEBLOOD_API AEnemyManager : public AActor
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
    class UBoxComponent* EM_CollisionBox;

public:

    //*******************************************************************************************************************
    //                                          CONSTRUCTOR && PUBLIC VARIABLES                                         *
    //*******************************************************************************************************************

    //Constructor
    AEnemyManager();

    bool bAllEnemiesAreDead;

    UPROPERTY(EditAnywhere, Category = "Enemy Data")
    TArray<class AShooterEnemy*> EnemiesList;

    //*******************************************************************************************************************
    //                                          PUBLIC FUNCTIONS                                                        *
    //*******************************************************************************************************************

    /*Removes one enemy from the list and checks if all the enemies are dead.*/
    void OnEnemyKilled(class AShooterEnemy* KilledEnemy);

    /*Used in ending cinematic in case that we want to kill all enemies in this level.*/
    UFUNCTION(BlueprintCallable)
    void ForceKillAllEnemies();

protected:

    //*******************************************************************************************************************
    //                                          PROTECTED VARIABLES                                                     *
    //*******************************************************************************************************************

    FTimerHandle CombatAudio_TimerHandle;
    FTimerHandle EndLevelTrigger_TimerHandle;
    FTimerHandle SceneBlocker_TimerHandle;

    const float MinCombatAudioTimerRate = 2.5f;
    const float MaxCombatAudioTimerRate = 4.0f;
    const float EndLevelLoopTimerRate   = 1.0f;
    const float SceneBlockerLoopRate    = 1.0f;

    float CombatAudioTimerRate;

    bool bOnForceKillAllEnemies;

    //*******************************************************************************************************************
    //                                          PROTECTED FUNCTIONS                                                     *
    //*******************************************************************************************************************

    virtual void BeginPlay() override;

    /*Takes one of the random enemies in the list and tell him to play some combat audio.
    If the enemy is NOT in combat state, it will just ignore this call.
    Otherwise, every "X" time some enemy will say something.*/
    UFUNCTION()
    void ChooseOneEnemyToPlayCombatAudio();
    /*Allows the player to overlap the EndLevelTriggerBox only if all near enemies are dead.*/
    UFUNCTION()
    void CheckIfPlayerCanAdvanceToNextLevel();
    /*Disable the scene blocking volume if all near enemies e dead.*/
    UFUNCTION()
    void CheckIfPlayerCanDisableBlockingVolume();
    /*Begin overlap Dynamic function*/
    UFUNCTION()
    void EM_BoxCollision_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);    

private:

    //*******************************************************************************************************************
    //                                          PRIVATE VARIABLES                                                       *
    //*******************************************************************************************************************

    UPROPERTY(EditAnywhere, Category = "Level completed")
    class ATriggerBox* EndLevelTriggerBox;
    UPROPERTY(EditAnywhere, Category = "Level completed")
    class ABlockingVolume* SceneBlocker;

    UPROPERTY(EditAnywhere, Category = "Level completed")
    class ATextRenderActor* EndLevelText;
    UPROPERTY(EditAnywhere, Category = "Level completed")
    class ATextRenderActor* SceneBlockerText;

    bool bEnemiesAlreadySpawned;

    /*If true, it will activate the final level trigger when all enemies are dead*/
    UPROPERTY(EditAnywhere, Category = "Enemy Data")
    bool bCanActivateFinalLevelTrigger;
    /*If true, it will deactivate the SceneBlocker BlockingVolume variable.*/
    UPROPERTY(EditAnywhere, Category = "Enemy Data")
    bool bCanBlockTheLevel;
    /*If true, it will disable enemies on begin play and enable them only when the player overlaps with EM_CollisionBox.*/
    UPROPERTY(EditAnywhere, Category = "Enemy Data")
    bool bShouldSpawnEnemies;

    //*******************************************************************************************************************
    //                                          PRIVATE FUNCTIONS                                                       *
    //*******************************************************************************************************************

    /*Spawn enemies assigned in EnemiesList*/
    void SpawnEnemiesAssigned();

};