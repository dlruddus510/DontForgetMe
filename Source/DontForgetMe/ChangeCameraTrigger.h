// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "ChangeCameraTrigger.generated.h"

UCLASS()
class DONTFORGETME_API AChangeCameraTrigger : public AActor
{
    GENERATED_BODY()

public:
    AChangeCameraTrigger();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    /*UPROPERTY(VisibleAnywhere, Category = "Platform")
        class UStaticMeshComponent* PlatformMesh;*/

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
        UBoxComponent* TriggerBox;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
        ACameraActor* CameraToSwitchTo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test")
        int32 TestInt;

    UFUNCTION()
        void OnOverlapBegin(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
        void OnOverlapEnd(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
