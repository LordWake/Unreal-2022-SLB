//+---------------------------------------------------------+
//| Project    : Smells Like Blood                          |
//| UE Version : UE 4.26                                    |
//| Author     : github.com/LordWake                        |
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SmellsLikeBlood/Interfaces/CoverInterface.h"

#include "BaseCoverObject.generated.h"

UCLASS()
class SMELLSLIKEBLOOD_API ABaseCoverObject : public AActor, public ICoverInterface
{
    GENERATED_BODY()

public:

    //*******************************************************************************************************************
    //                                          CONSTRUCTOR && COMPONENTS                                               *
    //*******************************************************************************************************************

    //!Constructor
    ABaseCoverObject();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    class USceneComponent* SceneRoot;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    class UStaticMeshComponent* CoverObject;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    class UBoxComponent* CoverBox1;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    class UArrowComponent* Arrow1;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    class UBoxComponent* EndOfCover_1_Right;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    class UBoxComponent* EndOfCover_1_Left;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    class UBoxComponent* CoverBox2;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    class UArrowComponent* Arrow2;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    class UBoxComponent* EndOfCover_2_Right;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    class UBoxComponent* EndOfCover_2_Left;

    //*******************************************************************************************************************
    //                                          PUBLIC FUNCTIONS                                                        *
    //*******************************************************************************************************************

    void SetCoverIsUsed(const bool& bCoverUsed);

protected:

    //*******************************************************************************************************************
    //                                          PROTECTED VARIABLES                                                     *
    //*******************************************************************************************************************

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    bool bCoverIsBeingUsed;

    //*******************************************************************************************************************
    //                                          PROTECTED FUNCTIONS                                                     *
    //*******************************************************************************************************************

    virtual void BeginPlay() override;

    #pragma region Overlap Dynamic Binding Functions
    UFUNCTION()
    void CoverBox_One_BeginOverlap(UPrimitiveComponent* OverlappedComponent,	AActor* OtherActor,	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    UFUNCTION()
    void CoverBox_Two_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    UFUNCTION()
    void EndOfCoverBox_Left_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    UFUNCTION()
    void EndOfCoverBox_Right_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    UFUNCTION()
    void CoverBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
    UFUNCTION()
    void EndOfCoverBox_EndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
    #pragma endregion

    /*Called when the Target enters on the CoverBox1 or CoverBox2 to tell that there is a cover available.*/
    void TellPlayerIfCoverIsAvailable(AActor* CheckThisActor, bool bIsTheCoverAvailable, FRotator CoverRotation);
    /*Called from the End_Of_Cover boxes to tell the player that he is in the end of this cover object.*/
    void TellPlayerIsEndOfCover(AActor* CheckThisActor, bool bEndOfCoverLeft, bool bEndOfCoverRight);
};