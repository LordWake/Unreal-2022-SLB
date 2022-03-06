//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#include "TutorialRobot.h"

#include "Kismet/GameplayStatics.h"
#include "SmellsLikeBlood/Tutorial/TutorialManager.h"
#include "Sound/SoundCue.h"

ATutorialRobot::ATutorialRobot()
{
    PrimaryActorTick.bCanEverTick = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ATutorialRobot::RobotTakeDamage()
{
    if (bRobotNotTakingDamage)
    {
        return;
    }

    if (!bHasBeenDestroyed)
    {
        MaxHealth--;
        if (MaxHealth <= 0)
        {
            bHasBeenDestroyed = true;

            if (!bMeleeRobot)
            {
                UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionVFX, GetActorLocation(), FRotator::ZeroRotator, FVector(1.f), true, EPSCPoolMethod::AutoRelease, true);
                UGameplayStatics::PlaySoundAtLocation(GetWorld(), DestroyedRobotCue, GetActorLocation());
            }
            else
            { 
                UGameplayStatics::PlaySoundAtLocation(GetWorld(), DestroyedMeleeRobotCue, GetActorLocation()); 
            }

            if (!bMeleeRobot && !bSlowTimeRobot)
            {
                TutorialManager->TutorialRobotDead(); 
            }
            else if (bMeleeRobot)
            {
                TutorialManager->TutorialMeleeRobotDead(); 
            }
            else
            {
                TutorialManager->TutorialSlowTimeRobotDead(); 
            }

            RobotIsDead();
        }
    }
}