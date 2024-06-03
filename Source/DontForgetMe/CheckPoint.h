
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheckPoint.generated.h"

UCLASS()
class DONTFORGETME_API ACheckPoint : public AActor
{
	GENERATED_BODY()
	
public:	

	ACheckPoint();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

};
