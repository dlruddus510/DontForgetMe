// Copyright Epic Games, Inc. All Rights Reserved.

#include "ITTGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnPoint.h"
#include "GameFramework/PlayerController.h"

AITTGameMode::AITTGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
        BP_RespawnCharacterClass = DefaultPawnClass;
	}
}

void AITTGameMode::StartPlay()
{
	Super::StartPlay();
    if (BP_RespawnCharacterClass == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("BP_RespawnCharacterClass is nullptr."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Spawning character with class: %s"), *BP_RespawnCharacterClass->GetName());
    }
}

void AITTGameMode::PlayerDied(AController* PlayerController)
{
    if (PlayerController != nullptr)
    {
        // ��Ȱ ī��Ʈ ����
        int32& Count = RespawnCounts.FindOrAdd(PlayerController);
        Count++;

        // ��Ȱ ��� �ð� ���
        int32 RespawnTime = FMath::Min(Count * 5.0f, 30.0f);

        // ��Ȱ ����
        FTimerHandle UnusedHandle;
        GetWorld()->GetTimerManager().SetTimer(UnusedHandle, [this, PlayerController]() {
            RespawnPlayer(PlayerController);
            }, RespawnTime, false);
    }
}

void AITTGameMode::RespawnPlayer(AController* PlayerController)
{
    if (PlayerController != nullptr)
    {
        UClass* RespawnCharacterClass = AITTCharacter::StaticClass();
        FVector SpawnLocation = FVector::ZeroVector;
        FRotator SpawnRotation = FRotator::ZeroRotator;

        // SpawnPoint ���͵��� ã��
        TArray<AActor*> SpawnPoints;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnPoint::StaticClass(), SpawnPoints);

        // ĳ���� Ŭ������ �´� SpawnPoint ã��
        for (AActor* SpawnPoint : SpawnPoints)
        {
            ASpawnPoint* MySpawnPoint = Cast<ASpawnPoint>(SpawnPoint);
            if (MySpawnPoint != nullptr && MySpawnPoint->IsSuitableFor(RespawnCharacterClass))
            {
                SpawnLocation = MySpawnPoint->GetActorLocation();
                SpawnRotation = MySpawnPoint->GetActorRotation();
                break; // ������ SpawnPoint�� ã�����Ƿ� �ݺ� �ߴ�
            }
        }

        // ������ SpawnPoint�� ã�� ���� ���, �⺻ ��ġ ���
        if (SpawnLocation.IsZero())
        {
            SpawnLocation = FVector(0.0f, 0.0f, 100.0f);
            SpawnRotation = FRotator(0.0f, 0.0f, 0.0f);
        }

        AITTCharacter* NewCharacter = GetWorld()->SpawnActor<AITTCharacter>(BP_RespawnCharacterClass, SpawnLocation, SpawnRotation);
        if (NewCharacter != nullptr)
        {
            NewCharacter->MaxHealth = 5.0f;
            NewCharacter->CurrentHealth = 5.0f; // ��Ȱ�� ĳ������ ü���� 5�� ����
            PlayerController->Possess(NewCharacter);
            NewCharacter->UpdateHealth();
            UE_LOG(LogTemp, Error, TEXT("Respawn"));
        }
    }
}
