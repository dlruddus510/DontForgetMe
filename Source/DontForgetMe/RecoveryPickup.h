// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "ITTCharacter.h"
#include "SecondPlayer.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Actor.h"
#include "RecoveryPickup.generated.h"

UCLASS()
class ITT_API ARecoveryPickup : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ARecoveryPickup();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // �浹�� �����ϱ� ���� ������Ʈ
    UPROPERTY(VisibleAnywhere, Category = "Components")
    class UBoxComponent* CollisionBox;

    // �浹 �� ����� �Լ�
    UFUNCTION()
   virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};