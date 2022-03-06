//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SmellsLikeBlood/Interfaces/DamageInterface.h"
#include "BaseProjectile.generated.h"

UCLASS()
class SMELLSLIKEBLOOD_API ABaseProjectile : public AActor, public IDamageInterface
{
    GENERATED_BODY()

    //*******************************************************************************************************************
    //                                          CLASS COMPONENTS                                                        *
    //*******************************************************************************************************************

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
    class USphereComponent* Bullet;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
    class UParticleSystemComponent* ParticleSystem;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
    class UProjectileMovementComponent* ProjectileMovement;

public:

    //*******************************************************************************************************************
    //                                         CONSTRUCTOR && PUBLIC VARIABLES                                          *
    //*******************************************************************************************************************

    //Constructor
    ABaseProjectile();

    float _MinDamage;
    float _MaxDamage;
    float _CriticChance;

    //*******************************************************************************************************************
    //                                          PUBLIC FUNCTIONS                                                        *
    //*******************************************************************************************************************

    /*Called from BaseWeapon or from Enemy Blueprint to set this bullet damage amount.*/
    UFUNCTION(BlueprintCallable)
    void SetDamageInfo(float MinDamage, float MaxDamage, float CriticChance);

    /*If it is spawned by the player, it will play rumble joystick feedback.*/
    void SpawnedByPlayer();

protected:

    //*******************************************************************************************************************
    //                                          PROTECTED VARIABLES                                                     *
    //*******************************************************************************************************************

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    class USoundBase* ImpactSound;
    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    class USoundBase* BodyImpactSound;
    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    class UParticleSystem* ImpactVFX;

    //*******************************************************************************************************************
    //                                          PROTECTED FUNCTIONS                                                     *
    //*******************************************************************************************************************

    virtual void BeginPlay() override;

    /*Called when hits something from Blueprints. Makes damage, spawn particles and play SFX.*/
    UFUNCTION(BlueprintCallable, Category = "BulletHit")
    virtual void BulletHit(const FHitResult& BulletHitResult);
};