// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ITTCharacter.h"
#include "BearCharacter.generated.h"

/**
 * 
 */
UCLASS()
class DONTFORGETME_API ABearCharacter : public AITTCharacter
{
	GENERATED_BODY()


	ABearCharacter();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	IInterfaceGrip* GrippableActor;

	void GripAndDrop(); //���&����

	void Throw(); //������

	
protected:
	AActor* AttachedActor = nullptr;

	AActor* PreviewBlock = nullptr;

public:
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Grip")
	bool bisGrip;  //�ִϸ��̼��� ���� �ӽú���
};
