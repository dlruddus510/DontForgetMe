// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ITTCharacter.h"
#include "ITTGameMode.generated.h"

UCLASS(minimalapi)
class AITTGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AITTGameMode();
	virtual void StartPlay() override; // ���� ���� �� ȣ��Ǵ� �Լ� �������̵�

	   // �÷��̾ �׾��� �� ȣ��Ǵ� �Լ�
	void PlayerDied(AController* LostPlayerController);

	// �÷��̾ ��Ȱ��Ű�� �Լ�
	void RespawnPlayer(AController* LostPlayerController);

	//ĳ������ �������Ʈ
	UPROPERTY(EditDefaultsOnly, Category = "Respawn")
	TSubclassOf<AITTCharacter> BP_RespawnCharacterClass;

	// �÷��̾��� ��Ȱ ī��Ʈ�� �����ϴ� ��
	TMap<AController*, int32> RespawnCounts;
};



