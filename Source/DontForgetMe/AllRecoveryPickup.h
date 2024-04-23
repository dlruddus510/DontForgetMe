// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RecoveryPickup.h"
#include "AllRecoveryPickup.generated.h"

/**
 * 
 */
UCLASS()
class ITT_API AAllRecoveryPickup : public ARecoveryPickup
{
	GENERATED_BODY()
	
public:
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

};
