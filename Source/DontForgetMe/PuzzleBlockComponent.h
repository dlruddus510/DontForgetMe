// PuzzleBlockComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PuzzleBlockComponent.generated.h"

UENUM(BlueprintType)
enum class EPuzzleBlockShape : uint8
{
    Shape1 UMETA(DisplayName = "Shape1"),
    Shape2 UMETA(DisplayName = "Shape2"),
    Shape3 UMETA(DisplayName = "Shape3"),
    // Add more shapes as needed
};

class APuzzleBoard; // ���� ����

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DONTFORGETME_API UPuzzleBlockComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPuzzleBlockComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = "PuzzleBlock")
        bool IsInPlace() const;

    UFUNCTION(BlueprintCallable, Category = "PuzzleBlock")
        bool IsOverlap() const;

protected:
    virtual void BeginPlay() override;

public:
    APuzzleBoard* OverlappingBoard; // �������� ���񺸵带 ����Ű�� ������ �߰�

private:
    FVector InitialLocation;
    FRotator InitialRotation;
    bool bInPlace;

    bool bisOverlap;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PuzzleBlock", meta = (AllowPrivateAccess = "true"))
        EPuzzleBlockShape BlockShape;

    void CheckIfInPlace();
    UFUNCTION()
        void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult); // ������ �̺�Ʈ �ڵ鷯 �߰�
};
