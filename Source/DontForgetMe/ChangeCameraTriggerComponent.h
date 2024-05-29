// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "Components/ActorComponent.h"
#include "ChangeCameraTriggerComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DONTFORGETME_API UChangeCameraTriggerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UChangeCameraTriggerComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    /*UPROPERTY(VisibleAnywhere, Category = "Platform")
        class UStaticMeshComponent* PlatformMesh;*/

        //UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
        //    UBoxComponent* TriggerBox;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
        ACameraActor* CameraToSwitchTo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test")
        int32 TestInt;

    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Delay")
    //FTimerHandle Delay;

    UFUNCTION(BlueprintCallable, Category = "ChangeCamera")
        void ChangeCamera();

    UFUNCTION(BlueprintCallable, Category = "ChangeCamera")
        void EndChangeCamera();
};
