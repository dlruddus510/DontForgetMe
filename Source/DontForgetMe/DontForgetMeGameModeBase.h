// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ITTCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "CheckPoint.h"
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

	// �÷��̾ ��Ȱ��Ű�� �Լ�
	void RespawnPlayer(AController* LostPlayerController);

	UClass* RespawnCharacterClass;

	AITTCharacter* ITTCharacter;

	// �÷��̾��� ��Ȱ ī��Ʈ�� �����ϴ� ��
	TMap<AController*, int32> RespawnCounts;

	UPROPERTY(EditDefaultsOnly, Category = "Respawn")
		TSubclassOf<AITTCharacter> BP_RespawnCharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
		TSubclassOf<AITTCharacter> BearCharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
		TSubclassOf<AITTCharacter> RobotCharacterClass;
};
