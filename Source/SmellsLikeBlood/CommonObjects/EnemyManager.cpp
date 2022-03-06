//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#include "EnemyManager.h"

#include "Engine/BlockingVolume.h"
#include "Engine/TriggerBox.h"
#include "Engine/TextRenderActor.h"

#include "Components/BoxComponent.h"

#include "Kismet/GameplayStatics.h"

#include "SmellsLikeBlood/Enemies/ShooterEnemy.h"
#include "SmellsLikeBlood/Player/TPS_PlayerCharacter.h"

AEnemyManager::AEnemyManager()
{
    PrimaryActorTick.bCanEverTick = true;
    EM_CollisionBox = CreateDefaultSubobject<UBoxComponent>("BoxCollision");
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AEnemyManager::OnEnemyKilled(class AShooterEnemy* KilledEnemy)
{
    if (bOnForceKillAllEnemies)
    {
        return;
    }

    EnemiesList.Remove(KilledEnemy);
    if (EnemiesList.Num() == 0)
    {
        bAllEnemiesAreDead = true;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AEnemyManager::ForceKillAllEnemies()
{
    bOnForceKillAllEnemies = true;
    bAllEnemiesAreDead     = true;

    for (const auto& ThisEnemy : EnemiesList)
    {
        if (UKismetSystemLibrary::DoesImplementInterface(ThisEnemy, UDamageInterface::StaticClass()))
        {
            const float DamageExtreme    = 1000.0f;
            const float DamageFullChance = 100.0f;

            IDamageInterface::Execute_TakeDamage(ThisEnemy, DamageExtreme, DamageExtreme, DamageFullChance, ThisEnemy->HeadBoneNameReference(), this);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AEnemyManager::BeginPlay()
{
    Super::BeginPlay();

    if (EnemiesList.Num() > 0)
    {
        for (const auto& ThisEnemy : EnemiesList)
        {
            ThisEnemy->SetEnemyManager(this);
            if (bShouldSpawnEnemies)
            {
                ThisEnemy->SetActorHiddenInGame(true);
                ThisEnemy->SetActorEnableCollision(false);
                ThisEnemy->bAIIsEnable = false;
            }
        }
    }

    EM_CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemyManager::EM_BoxCollision_BeginOverlap); //Add UFUNCTION to manager's box trigger

    CombatAudioTimerRate = FMath::RandRange(MinCombatAudioTimerRate, MaxCombatAudioTimerRate);
    GetWorldTimerManager().SetTimer(CombatAudio_TimerHandle, this, &AEnemyManager::ChooseOneEnemyToPlayCombatAudio, CombatAudioTimerRate, false, CombatAudioTimerRate);

    #pragma region EndLevelTriger & BlockingVolume
    if ((bCanActivateFinalLevelTrigger) && (EndLevelTriggerBox != nullptr) && (EndLevelText != nullptr))
    {
        EndLevelTriggerBox->SetActorEnableCollision(false);
        EndLevelText->SetActorHiddenInGame(false);
        GetWorldTimerManager().SetTimer(EndLevelTrigger_TimerHandle, this, &AEnemyManager::CheckIfPlayerCanAdvanceToNextLevel, EndLevelLoopTimerRate, true, EndLevelLoopTimerRate);
    }

    if ((bCanBlockTheLevel) && (SceneBlocker != nullptr) && (SceneBlockerText != nullptr))
    {
        GetWorldTimerManager().SetTimer(SceneBlocker_TimerHandle, this, &AEnemyManager::CheckIfPlayerCanDisableBlockingVolume, SceneBlockerLoopRate, true, SceneBlockerLoopRate);
    }
    #pragma endregion
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AEnemyManager::ChooseOneEnemyToPlayCombatAudio()
{
    if ((!bAllEnemiesAreDead) && (!bShouldSpawnEnemies && bEnemiesAlreadySpawned))
    {
        if (EnemiesList.Num() > 0)
        {
            const uint8_t RandomEnemy = FMath::RandRange(0, EnemiesList.Num() - 1);
            EnemiesList[RandomEnemy]->PlayCombatAudio();
        }
    }

    CombatAudioTimerRate = FMath::RandRange(MinCombatAudioTimerRate, MaxCombatAudioTimerRate);
    GetWorldTimerManager().SetTimer(CombatAudio_TimerHandle, this, &AEnemyManager::ChooseOneEnemyToPlayCombatAudio, CombatAudioTimerRate, false, CombatAudioTimerRate);
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AEnemyManager::CheckIfPlayerCanAdvanceToNextLevel()
{
    if (bAllEnemiesAreDead)
    {
        EndLevelTriggerBox->SetActorEnableCollision(true);
        EndLevelText->SetActorHiddenInGame(true);
        GetWorldTimerManager().ClearTimer(EndLevelTrigger_TimerHandle);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AEnemyManager::CheckIfPlayerCanDisableBlockingVolume()
{
    if (bAllEnemiesAreDead)
    {
        SceneBlocker->SetActorEnableCollision(false);
        SceneBlockerText->SetActorHiddenInGame(true);
        GetWorldTimerManager().ClearTimer(SceneBlocker_TimerHandle);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AEnemyManager::EM_BoxCollision_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (Cast<ATPS_PlayerCharacter>(OtherActor))
    {
        if ((bEnemiesAlreadySpawned) || (!bShouldSpawnEnemies))
        {
            return;
        }

        SpawnEnemiesAssigned();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
void AEnemyManager::SpawnEnemiesAssigned()
{
    bEnemiesAlreadySpawned = true;
    bShouldSpawnEnemies    = false;

    for (const auto& ThisEnemy : EnemiesList)
    {
        ThisEnemy->SetActorHiddenInGame(false);
        ThisEnemy->SetActorEnableCollision(true);
        ThisEnemy->bAIIsEnable = true;
    }
}