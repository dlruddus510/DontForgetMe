// Copyright Epic Games, Inc. All Rights Reserved.


#include "DontForgetMeGameModeBase.h"

void ADontForgetMeGameModeBase::PlayerDied(AController* PlayerController)
{
    if (PlayerController != nullptr)
    {
        // ��Ȱ ī��Ʈ ����
        int32& Count = RespawnCounts.FindOrAdd(PlayerController);
        Count++;

        // ��Ȱ ��� �ð� ���
        RespawnTime = FMath::Min(Count * 5.0f, 30.0f);
        RespawnCharacterClass = nullptr;
        APawn* ControlledPawn = PlayerController->GetPawn();
        if (ControlledPawn)
        {
            ITTCharacter = Cast<AITTCharacter>(ControlledPawn);
            if (ITTCharacter)
            {
                switch (ITTCharacter->SelectedCharacterType)
                {
                case ECharacterType::Bear:
                    RespawnCharacterClass = BearCharacterClass;
                    break;
                case ECharacterType::Robot:
                    RespawnCharacterClass = RobotCharacterClass;
                    break;
                default:
                    RespawnCharacterClass = AITTCharacter::StaticClass();
                    break;
                }

                // �� �÷��̾ ���� UI ����
                ITTCharacter->PlayRespawnUI();

                ITTCharacter->Destroy();
            }
        }

        // �÷��̾ ���� UI�� PlayerRespawn�� ����
        PlayerRespawn = ITTCharacter->PlayerRespawn;

        // ��Ȱ ����
        FTimerHandle& PlayerTimerHandle = PlayerTimerHandles.FindOrAdd(PlayerController);
        GetWorld()->GetTimerManager().SetTimer(PlayerTimerHandle, [this, PlayerController]() {
            RespawnPlayer(PlayerController);
            HideRespawnUI(PlayerController);
            }, RespawnTime, false);
    }
}
void ADontForgetMeGameModeBase::RespawnPlayer(AController* PlayerController)
{
    if (PlayerController != nullptr)
    {
        FVector SpawnLocation = FVector::ZeroVector;
        FRotator SpawnRotation = FRotator::ZeroRotator;
        APawn* ControlledPawn = PlayerController->GetPawn();
        if (ITTCharacter)
        {
            SpawnLocation = ITTCharacter->CheckPoint;

            AITTCharacter* NewCharacter = GetWorld()->SpawnActor<AITTCharacter>(RespawnCharacterClass, SpawnLocation, SpawnRotation);
            if (NewCharacter != nullptr)
            {
                NewCharacter->MaxHealth = 5.0f;
                NewCharacter->CurrentHealth = 5.0f; // ��Ȱ�� ĳ������ ü���� 5�� ����
                PlayerController->Possess(NewCharacter);
                NewCharacter->UpdateHealth();
                UE_LOG(LogTemp, Error, TEXT("Respawn: Successfully respawned a character.")); // ��Ȱ ���� �� �α�
            }
        }
    }
}

float ADontForgetMeGameModeBase::GetRemainingTime(AController* PlayerController)
{
    FTimerHandle* PlayerTimerHandle = PlayerTimerHandles.Find(PlayerController);
    if (PlayerTimerHandle != nullptr)
    {
        return GetWorld()->GetTimerManager().GetTimerRemaining(*PlayerTimerHandle);
    }
    return 0.0f;
}

void ADontForgetMeGameModeBase::HideRespawnUI(AController* PlayerController)
{
    // �÷��̾ UI �����
    if (PlayerRespawn != nullptr)
    {
        PlayerRespawn->RemoveFromParent();
        PlayerRespawn = nullptr;
    }
}