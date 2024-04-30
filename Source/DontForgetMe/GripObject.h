// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BearCharacter.h"
#include "InterfaceGrip.h"
#include "GripObject.generated.h"

UCLASS()
class DONTFORGETME_API AGripObject : public AActor, public IInterfaceGrip
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGripObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	AActor* TargetActor;
	FVector TargetLocation;
	bool bIsMovingToTarget;


	void BeginMoveToTarget(AActor* NewTargetActor, FVector NewTargetLocation);
	void StopMoveToTarget();

	/*UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* StaticMeshComp;*/

	UFUNCTION(BlueprintNativeEvent, Category = "InterfaceAction")
	void Grip();
	void Grip_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "InterfaceAction")
	void Drop();
	void Drop_Implementation();
};
