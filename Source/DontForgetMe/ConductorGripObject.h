// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GripObject.h"
#include "ConductorInterface.h"
#include "ConductorGripObject.generated.h"

/**
 * 
 */
UCLASS()
class DONTFORGETME_API AConductorGripObject : public AGripObject, public IConductorInterface
{
	GENERATED_BODY()
	
public:

	AConductorGripObject();

	void BeginPlay() override;

	void Tick(float DeltaTime) override;

	//UPROPERTY(BlueprintReadWrite, Category = "Electricity")
	bool electricityStatus;

	UParticleSystemComponent* PSC;

	FTimerHandle ParticleSystemDeactivationTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* ElectricEffect; // ���� ��ƼŬ �ý��ۿ� ���� ����

	UFUNCTION(BlueprintCallable, Category = "Electricity")
	virtual void ConductElectricity() override;

	UFUNCTION(BlueprintCallable, Category = "Electricity")
	virtual void StopConductElectricity() override;

	virtual void DeactivateAndDestroyParticleSystem() override;


};