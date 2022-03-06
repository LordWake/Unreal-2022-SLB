//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosiveBarrel.generated.h"

UCLASS()
class SMELLSLIKEBLOOD_API AExplosiveBarrel : public AActor
{
    GENERATED_BODY()

public:

    //*******************************************************************************************************************
    //                                          CONSTRUCTOR && PUBLIC VARIABLES                                          *
    //*******************************************************************************************************************

    //Constructor
    AExplosiveBarrel();

    class USceneComponent* SceneRoot;

    UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
    class UStaticMeshComponent* BarrelMesh;
    UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
    class UParticleSystemComponent* FireVFX;

protected:

    //*******************************************************************************************************************
    //                                          PROTECTED VARIABLES                                                     *
    //*******************************************************************************************************************

    UPROPERTY(EditDefaultsOnly, Category = "VFX")
    UParticleSystem* ExplosionVFX;
    UPROPERTY(EditDefaultsOnly, Category = "SFX")
    class USoundCue* ExplosionCue;

    //*******************************************************************************************************************
    //                                          PROTECTED FUNCTIONS                                                     *
    //*******************************************************************************************************************

    virtual void BeginPlay() override;

    UFUNCTION()
    void OnBarrelHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

    /*Used in BP to break the DestructibleMesh.*/
    UFUNCTION(BlueprintImplementableEvent)
    void OnMeshExplosion();

private:

    //*******************************************************************************************************************
    //                                          PRIVATE VARIABLES                                                       *
    //*******************************************************************************************************************

    UPROPERTY(EditDefaultsOnly, Category = "Explosion Damage")
    float ExplosiveBarrel_EnemyDamage;
    UPROPERTY(EditDefaultsOnly, Category = "Explosion Damage")
    float ExplosiveBarrel_PlayerDamage;

    bool bBarrelDestroyed;

    //*******************************************************************************************************************
    //                                          PRIVATE FUNCTIONS                                                       *
    //*******************************************************************************************************************

    /*Destroys the StaticMesh, and play SFX/VFX. It will also call another ExplosiveBarrel if they are near.*/
    void OnBarrelExplosion();
};