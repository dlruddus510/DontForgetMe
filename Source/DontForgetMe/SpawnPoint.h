// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnPoint.generated.h"

UCLASS()
class ITT_API ASpawnPoint : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ASpawnPoint();

    // Ư�� ĳ���� Ŭ������ �������� ���θ� �Ǵ��ϴ� �Լ�
    UFUNCTION(BlueprintCallable, Category = "SpawnPoint")
        bool IsSuitableFor(UClass* CharacterClass) const;
};