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
        int32 RespawnTime = FMath::Min(Count * 5.0f, 30.0f);
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

                // ĳ���� Ŭ������ �´� SpawnPoint ã��
                ITTCharacter->Destroy();
            }
        }

        // ��Ȱ ����
        FTimerHandle UnusedHandle;
        GetWorld()->GetTimerManager().SetTimer(UnusedHandle, [this, PlayerController]() {
            RespawnPlayer(PlayerController);
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

            //if (ControlledPawn)
            //{
            //    AITTCharacter* ITTCharacter = Cast<AITTCharacter>(ControlledPawn);
            //    if (ITTCharacter)
            //    {
            //        switch (ITTCharacter->SelectedCharacterType)
            //        {
            //        case ECharacterType::Bear:
            //            RespawnCharacterClass = BearCharacterClass;
            //            break;
            //        case ECharacterType::Robot:
            //            RespawnCharacterClass = RobotCharacterClass;
            //            break;
            //        default:
            //            RespawnCharacterClass = AITTCharacter::StaticClass();
            //            break;
            //        }

            //        // ĳ���� Ŭ������ �´� SpawnPoint ã��
            //        ITTCharacter->Destroy();
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
