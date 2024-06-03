
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


UCLASS()
class DONTFORGETME_API ADontForgetMeGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	void PlayerDied(AController* LostPlayerController);


	void RespawnPlayer(AController* PlayerController, TSubclassOf<AITTCharacter> RespawnCharacterClass);

	UClass* RespawnCharacterClass;

	AITTCharacter* ITTCharacter;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	TMap<AController*, int32> RespawnCounts;

	UFUNCTION(BlueprintCallable, Category = "Respawn")
		float GetRemainingTime(AController* PlayerController);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
		float RemainingTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	int32 RespawnTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	TMap<AController*, FTimerHandle> PlayerTimerHandles; 

		TMap<AController*, TWeakObjectPtr<UUserWidget>> PlayerRespawnMap;

	UPROPERTY(EditDefaultsOnly, Category = "Respawn")
		TSubclassOf<AITTCharacter> BP_RespawnCharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
		TSubclassOf<AITTCharacter> BearCharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
		TSubclassOf<AITTCharacter> RobotCharacterClass;

	void HideRespawnUI(AController* PlayerController); 
};
