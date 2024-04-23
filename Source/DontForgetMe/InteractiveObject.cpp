// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveObject.h"
#include "ITTCharacter.h"

// Sets default values
AInteractiveObject::AInteractiveObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInteractiveObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractiveObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractiveObject::Interact(ACharacter* Character)
{
	UE_LOG(LogTemp, Error, TEXT("Damage Test"));
	AITTCharacter* ITTCharacter = Cast<AITTCharacter>(Character);
	if (ITTCharacter) // �����Ͱ� ��ȿ���� Ȯ��
	{
		float DamageAmount = 1.0f;
		ITTCharacter->TakeDamage(DamageAmount, FDamageEvent(), nullptr, nullptr);
	}
}
