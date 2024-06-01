#include "DontForgetMeGameModeBase.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

void ADontForgetMeGameModeBase::PlayerDied(AController* PlayerController)
{
    if (PlayerController != nullptr)
    {
        // 부활 카운트 증가
        int32& Count = RespawnCounts.FindOrAdd(PlayerController);
        Count++;

        // 부활 대기 시간 계산
        float RespawnTimeLocal = FMath::Min(Count * 5.0f, 30.0f);
        TSubclassOf<AITTCharacter> RespawnCharacterClassLocal = nullptr;
        APawn* ControlledPawn = PlayerController->GetPawn();
        if (ControlledPawn)
        {
            ITTCharacter = Cast<AITTCharacter>(ControlledPawn);
            if (ITTCharacter)
            {
                switch (ITTCharacter->SelectedCharacterType)
                {
                case ECharacterType::Bear:
                    RespawnCharacterClassLocal = BearCharacterClass;
                    break;
                case ECharacterType::Robot:
                    RespawnCharacterClassLocal = RobotCharacterClass;
                    break;
                default:
                    RespawnCharacterClassLocal = AITTCharacter::StaticClass();
                    break;
                }

                // 각 플레이어에 대한 UI 설정
                ITTCharacter->PlayRespawnUI();

                ITTCharacter->Destroy();
            }
        }

        // 플레이어에 대한 UI를 PlayerRespawnMap에 저장
        TWeakObjectPtr<UUserWidget>& PlayerRespawn = PlayerRespawnMap.FindOrAdd(PlayerController);
        PlayerRespawn = ITTCharacter->PlayerRespawn;

        // 부활 로직
        FTimerHandle& PlayerTimerHandle = PlayerTimerHandles.FindOrAdd(PlayerController);
        GetWorld()->GetTimerManager().SetTimer(PlayerTimerHandle, [this, PlayerController, RespawnCharacterClassLocal]() {
            RespawnPlayer(PlayerController, RespawnCharacterClassLocal);
            HideRespawnUI(PlayerController);
            }, RespawnTimeLocal, false);
    }
}

void ADontForgetMeGameModeBase::RespawnPlayer(AController* PlayerController, TSubclassOf<AITTCharacter> RespawnCharacterClassLocal)
{
    if (PlayerController != nullptr)
    {
        FVector SpawnLocation = FVector::ZeroVector;
        FRotator SpawnRotation = FRotator::ZeroRotator;
        if (ITTCharacter)
        {
            SpawnLocation = ITTCharacter->CheckPoint;

            AITTCharacter* NewCharacter = GetWorld()->SpawnActor<AITTCharacter>(RespawnCharacterClassLocal, SpawnLocation, SpawnRotation);
            if (NewCharacter != nullptr)
            {
                NewCharacter->MaxHealth = 5.0f;
                NewCharacter->CurrentHealth = 5.0f; // 부활한 캐릭터의 체력을 5로 설정
                PlayerController->Possess(NewCharacter);
                NewCharacter->UpdateHealth();
                UE_LOG(LogTemp, Error, TEXT("Respawn: Successfully respawned a character.")); // 부활 성공 시 로그
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
    // 플레이어별 UI 숨기기
    TWeakObjectPtr<UUserWidget>* PlayerRespawnPtr = PlayerRespawnMap.Find(PlayerController);
    if (PlayerRespawnPtr != nullptr && PlayerRespawnPtr->IsValid())
    {
        UUserWidget* PlayerRespawn = PlayerRespawnPtr->Get();
        if (PlayerRespawn != nullptr)
        {
            PlayerRespawn->RemoveFromParent();
            *PlayerRespawnPtr = nullptr;
        }
    }
}
