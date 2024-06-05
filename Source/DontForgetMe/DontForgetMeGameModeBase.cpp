#include "DontForgetMeGameModeBase.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

void ADontForgetMeGameModeBase::PlayerDied(AController* PlayerController)
{
    if (PlayerController != nullptr)
    {
        int32& Count = RespawnCounts.FindOrAdd(PlayerController);
        Count++;

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

                ITTCharacter->PlayRespawnUI();

                ITTCharacter->Destroy();
            }
        }


        TWeakObjectPtr<UUserWidget>& PlayerRespawn = PlayerRespawnMap.FindOrAdd(PlayerController);
        PlayerRespawn = ITTCharacter->PlayerRespawn;


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
            FActorSpawnParameters SpawnParameters;
            SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            AITTCharacter* NewCharacter = GetWorld()->SpawnActor<AITTCharacter>(RespawnCharacterClassLocal, SpawnLocation, SpawnRotation, SpawnParameters);
           
            if (NewCharacter != nullptr)
            {
                NewCharacter->MaxHealth = 5.0f;
                NewCharacter->CurrentHealth = 5.0f; 
                PlayerController->Possess(NewCharacter);
                NewCharacter->UpdateHealth();
                UE_LOG(LogTemp, Error, TEXT("Respawn: Successfully respawned a character."));
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
