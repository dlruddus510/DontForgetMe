#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UpDown.generated.h"

UCLASS()
class DONTFORGETME_API AUpDown : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AUpDown();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
        UStaticMeshComponent* MeshComp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
        float MoveSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
        float MoveDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
        FVector MoveDirection;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target")
        AActor* TargetObject2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
        UMaterialInterface* NewMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
        UMaterialInterface* OriginalMaterial;

    UFUNCTION()
        void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION(BlueprintCallable, Category = "Movement")
        void MoveObjects();

    UFUNCTION(BlueprintCallable, Category = "Movement")
        void SetMoveDirection(FVector NewDirection);

private:
    bool bIsMoving;
    bool bIsReturning;
    FVector InitialLocation;
    FVector TargetLocation;
    FVector Object2InitialLocation;
    FTimerHandle ReturnTimerHandle;
    FTimerHandle MaterialTimerHandle;

    void StartReturnMovement();
    void ReturnObjects(float DeltaTime);
    void ResetObjects();
    void ResetMaterial();
};
