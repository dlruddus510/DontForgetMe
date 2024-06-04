#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UpDown.generated.h"

UCLASS()
class DONTFORGETME_API AUpDown : public AActor
{
    GENERATED_BODY()

public:
    AUpDown();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* MeshComp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MoveSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MoveDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    FVector MoveDirection;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
    UMaterialInterface* NewMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
    UMaterialInterface* OriginalMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
    UMaterialInterface* NewMaterial2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target")
    AActor* TargetObject2;

private:
    FVector InitialLocation;
    FVector TargetLocation;
    FVector Object2InitialLocation;

    bool bIsMoving;
    bool bIsReturning;
    bool bIsCooldown;

    FTimerHandle ReturnTimerHandle;
    FTimerHandle MaterialTimerHandle;
    FTimerHandle CooldownTimerHandle;

    void MoveObjects();
    void StartReturnMovement();
    void ReturnObjects(float DeltaTime);
    void ResetObjects();
    void ResetMaterial();
    void ResetCooldown();

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
    void SetMoveDirection(FVector NewDirection);
};
