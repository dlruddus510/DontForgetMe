#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Object1.generated.h"

UCLASS()
class DONTFORGETME_API AObject1 : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AObject1();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

private:
    UPROPERTY(VisibleAnywhere, Category = "Components")
        UStaticMeshComponent* MeshComp;

    UPROPERTY(EditAnywhere, Category = "Movement")
        float MoveSpeed;

    UPROPERTY(EditAnywhere, Category = "Movement")
        float MoveDistance;

    UPROPERTY(EditAnywhere, Category = "Materials")
        UMaterialInterface* NewMaterial;  // The new material to apply on overlap

    UMaterialInterface* OriginalMaterial;  // The original material to revert to

    FVector InitialLocation;
    FVector TargetLocation;
    FVector Object2InitialLocation;
    bool bIsMoving;
    bool bIsReturning;

    UFUNCTION()
        void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    FTimerHandle TimerHandle;  // 타이머 핸들 추가
    FTimerHandle MaterialTimerHandle;  // New timer handle for material reset
    FTimerHandle ReturnTimerHandle;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
        AActor* TargetObject2;

    void MoveObjects();
    void ResetObjects();
    void StartReturnMovement();  // New function to start the return movement
    void ReturnObjects(float DeltaTime);  // New function to handle returning movement
    void ResetMaterial();  // New function to reset the material

};
