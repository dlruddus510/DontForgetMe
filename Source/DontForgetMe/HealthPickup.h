// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RecoveryPickup.h"
#include "HealthPickup.generated.h"

/**
 * 
 */
UCLASS()
class DONTFORGETME_API AHealthPickup : public ARecoveryPickup
{
	GENERATED_BODY()
	
public:
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};
