#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Object2.generated.h"

UCLASS()
class DONTFORGETME_API AObject2 : public AActor
{
    GENERATED_BODY()

public:

    AObject2();

protected:

    virtual void BeginPlay() override;

public:
 
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
        UStaticMeshComponent* MeshComp;

    UFUNCTION(BlueprintCallable, Category = "Movement")
        void MoveObject(float Distance);

    UFUNCTION(BlueprintCallable, Category = "Movement")
        void SetMoveDirection(FVector NewDirection);

private:
    FVector MoveDirection;
};
