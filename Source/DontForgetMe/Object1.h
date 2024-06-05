#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Object1.generated.h"

UCLASS()
class DONTFORGETME_API AObject1 : public AActor
{
    GENERATED_BODY()

public:

    AObject1();

protected:

    virtual void BeginPlay() override;

public:

    virtual void Tick(float DeltaTime) override;

private:
    UPROPERTY(VisibleAnywhere, Category = "Components")
        UStaticMeshComponent* MeshComp;

    UPROPERTY(EditAnywhere, Category = "Movement")
        float MoveSpeed;

    UPROPERTY(EditAnywhere, Category = "Movement")
        float MoveDistance;

    UPROPERTY(EditAnywhere, Category = "Materials")
        UMaterialInterface* NewMaterial;  

    UMaterialInterface* OriginalMaterial; 

    FVector InitialLocation;
    FVector TargetLocation;
    FVector Object2InitialLocation;
    bool bIsMoving;
    bool bIsReturning;

    UFUNCTION()
        void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    FTimerHandle TimerHandle; 
    FTimerHandle MaterialTimerHandle;  
    FTimerHandle ReturnTimerHandle;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
        AActor* TargetObject2;

    void MoveObjects();
    void ResetObjects();
    void StartReturnMovement();  
    void ReturnObjects(float DeltaTime);  
    void ResetMaterial(); 

};
