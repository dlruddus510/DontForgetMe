// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ITTCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "CheckPoint.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraActor.h"
#include "DontForgetMeGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class DONTFORGETME_API ADontForgetMeGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	void PlayerDied(AController* LostPlayerController);

	// 플레이어를 부활시키는 함수
	void RespawnPlayer(AController* LostPlayerController);

	UClass* RespawnCharacterClass;

	AITTCharacter* ITTCharacter;

	// 플레이어의 부활 카운트를 저장하는 맵
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	TMap<AController*, int32> RespawnCounts;

	UFUNCTION(BlueprintCallable, Category = "Respawn")
		float GetRemainingTime(AController* PlayerController);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
		float RemainingTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	int32 RespawnTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	TMap<AController*, FTimerHandle> PlayerTimerHandles; // 추가: 플레이어마다 다른 타이머 핸들 저장

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
		UUserWidget* PlayerRespawn;

	UPROPERTY(EditDefaultsOnly, Category = "Respawn")
		TSubclassOf<AITTCharacter> BP_RespawnCharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
		TSubclassOf<AITTCharacter> BearCharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
		TSubclassOf<AITTCharacter> RobotCharacterClass;

	void HideRespawnUI(AController* PlayerController); // 추가: 플레이어마다 다른 UI 숨기기
};
