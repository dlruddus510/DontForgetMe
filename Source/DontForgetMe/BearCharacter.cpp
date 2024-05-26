// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFramework/CharacterMovementComponent.h"
#include "BearCharacter.h"


//��� ������
void ABearCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Grip", IE_Pressed, this, &ABearCharacter::GripAndDrop);

	PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &ABearCharacter::Throw);
}

void ABearCharacter::BeginPlay()
{
	Super::BeginPlay();

	bisGrip = false;
	
}

void  ABearCharacter::MaterialChange()
{
	USkeletalMeshComponent* MeshComponent = GetMesh();
	if (MeshComponent)
	{
		int32 NumMaterials = MeshComponent->GetNumMaterials();
		for (int32 i = 0; i < NumMaterials; ++i)
		{
			UMaterialInstanceDynamic* DynamicMaterial = MeshComponent->CreateAndSetMaterialInstanceDynamic(i);
			if (DynamicMaterial)
			{
				UE_LOG(LogTemp, Warning, TEXT("Set Opacity for material at index %d"), i);
				DynamicMaterial->SetScalarParameterValue(FName("Opacity"), 0.5f);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to create DynamicMaterial for index %d"), i);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("MeshComponent is null"));
	}
}

void ABearCharacter::GripAndDrop()
{
	if (!AttachedActor) // ��� ���� �ʴٸ�
	{
		if (!bIsStaminaDepleted && CurrentStamina >= 1)
		{
			FVector StartLocation = GetForwardLocation(10.0f);
			FVector EndLocation = GetForwardLocation(150.0f);

			if (CheckForObjectsInChannel(StartLocation, EndLocation, ECC_GameTraceChannel1, LocalCollisionShape))
			{
				AttachedActor = OutHit.GetActor();
				GrippableActor = Cast<IInterfaceGrip>(AttachedActor);

				if (GrippableActor && AttachedActor)
				{
					bisGrip = true;
					DecreaseStamina(1.0f);
					UE_LOG(LogTemp, Error, TEXT("DecreaseStamina"));
					GrippableActor->Grip();
					TargetLocation = GetMesh()->GetSocketLocation(TEXT("AttachPosition"));
					GrippableActor->BeginMoveToTarget(this, TargetLocation);
				}
			}
		}
	}
	else // �̹� ��� �ִٸ�
	{
		GrippableActor = Cast<IInterfaceGrip>(AttachedActor);
		if (GrippableActor)
		{
			FRotator NewRotation = FRotator(0.f, 0.f, 0.f);
			FVector DropLocation = GetForwardLocation(150.0f);

			/*FVector CurrentScale = AttachedActor->GetActorScale3D();
			FVector NewScale = CurrentScale * 2.0f;
			AttachedActor->SetActorScale3D(NewScale);*/

			AttachedActor->GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

			FVector StartLocation = GetForwardLocation(1.0f);
			FVector EndLocation = GetForwardLocation(50.0f);

			if (CheckForObjectsInChannel(StartLocation, EndLocation, ECC_GameTraceChannel1, LocalCollisionShape))
			{
				FVector HitLocation = OutHit.ImpactPoint;
				float ObjectHeight = OutHit.GetActor()->GetSimpleCollisionHalfHeight() * 4.0f;
				FVector CenterLocation = OutHit.GetActor()->GetActorLocation();

				NewRotation = OutHit.GetActor()->GetActorRotation();

				DropLocation.Z = HitLocation.Z + ObjectHeight + AttachedActor->GetSimpleCollisionHalfHeight();
				DropLocation.X = CenterLocation.X;
				DropLocation.Y = CenterLocation.Y;
			}
		
			AttachedActor->SetActorLocationAndRotation(DropLocation, NewRotation);
			AttachedActor = nullptr;
			GrippableActor->StopMoveToTarget();
			GrippableActor->Drop();
			bisGrip = false;
		}
	}
}

//
//void ABearCharacter::GripAndDrop()
//{
//	if (!AttachedActor) // ��� ���� �ʴٸ�
//	{
//		if (!bIsStaminaDepleted && CurrentStamina >= 1)
//		{
//			FVector StartLocation = GetForwardLocation(10.0f);
//			FVector EndLocation = GetForwardLocation(150.0f);
//
//			if (CheckForObjectsInChannel(StartLocation, EndLocation, ECC_GameTraceChannel1, LocalCollisionShape))
//			{
//				AttachedActor = OutHit.GetActor();
//				GrippableActor = Cast<IInterfaceGrip>(AttachedActor);
//
//				if (GrippableActor && AttachedActor)
//				{
//					DecreaseStamina(1.0f);
//					UE_LOG(LogTemp, Error, TEXT("DecreaseStamina"));
//					GrippableActor->Grip();
//					TargetLocation = GetMesh()->GetSocketLocation(TEXT("AttachPosition"));
//					GrippableActor->BeginMoveToTarget(this, TargetLocation);
//				}
//			}
//		}
//	}
//
//	else // �̹� ��� �ִٸ�
//
//	{
//		GrippableActor = Cast<IInterfaceGrip>(AttachedActor);
//		if (GrippableActor)
//		{
//			FRotator NewRotation = FRotator(0.f, 0.f, 0.f);
//			FVector DropLocation = GetForwardLocation(150.0f); // �⺻ ���� ��ġ
//
//			FVector CurrentScale = AttachedActor->GetActorScale3D();
//
//			// �������� 2��� �����մϴ�.
//			FVector NewScale = CurrentScale * 2.0f;
//
//			// ����� �������� ���Ϳ� �����մϴ�.
//			AttachedActor->SetActorScale3D(NewScale);
//
//			AttachedActor->GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
//
//			// �տ� ������Ʈ�� �ִ��� Ȯ��
//			FVector StartLocation = GetForwardLocation(1.0f);
//			FVector EndLocation = GetForwardLocation(50.0f); // ���� �� ���� Ȯ��
//
//			if (CheckForObjectsInChannel(StartLocation, EndLocation, ECC_GameTraceChannel1, LocalCollisionShape))
//			{
//				// �տ� ������Ʈ�� ������, �� ������Ʈ�� �߽����� ��ġ ����
//				FVector HitLocation = OutHit.ImpactPoint;
//				float ObjectHeight = OutHit.GetActor()->GetSimpleCollisionHalfHeight() * 4.0f; // �ش� ������Ʈ�� ��ü ���� ���
//				FVector CenterLocation = OutHit.GetActor()->GetActorLocation(); // �浹�� ������Ʈ�� �߽� ��ġ
//
//				NewRotation = OutHit.GetActor()->GetActorRotation();
//
//				// ���� ��ü�� �عٴ��� ������Ʈ ��ܿ� ������ Z �� ����
//				DropLocation.Z = HitLocation.Z + ObjectHeight + AttachedActor->GetSimpleCollisionHalfHeight();
//				// �浹�� ������Ʈ�� �߽����� X�� Y ���� ����
//				DropLocation.X = CenterLocation.X;
//				DropLocation.Y = CenterLocation.Y;
//			}
//
//			// Detach�ϰ� ��ġ ����
//			AttachedActor->SetActorLocation(DropLocation);
//			AttachedActor->SetActorRotation(NewRotation);
//
//			AttachedActor = nullptr;
//		}
//		GrippableActor->StopMoveToTarget();
//		GrippableActor->Drop();
//	}
//}

void ABearCharacter::Throw()
{
	if (AttachedActor && CurrentStamina >= 2 && !bIsStaminaDepleted)
	{
		// AttachedActor�� ���纻�� �����մϴ�.
		AActor* ClonedActor = GetWorld()->SpawnActor<AActor>(AttachedActor->GetClass(), AttachedActor->GetActorLocation(), AttachedActor->GetActorRotation());

		// ����� ���Ϳ� ������ �Ӽ� ����
		UPrimitiveComponent* RootPrimitiveComponent = Cast<UPrimitiveComponent>(ClonedActor->GetRootComponent());
		if (RootPrimitiveComponent)
		{
			// �������� �׸��� ���ư� �� �ֵ��� �ʱ� ��ݷ��� �߰��մϴ�.
			FVector LaunchVelocity = GetActorForwardVector() * 1000.0f;
			LaunchVelocity.Z += 500.0f; // ���� �������� �߰� �ӵ��� �ο��մϴ�.
			RootPrimitiveComponent->AddImpulse(LaunchVelocity, NAME_None, true);

			RootPrimitiveComponent->SetSimulatePhysics(true);
		}

		// ���¹̳� ����
		DecreaseStamina(2.0f);

		// ������ ��� �ִ� ���͸� �����մϴ�.
		AttachedActor->Destroy();

		// ���� �ʱ�ȭ
		AttachedActor = nullptr;
		GrippableActor = nullptr;
	}
}


//void ABearCharacter::Throw()
//{
//	if (AttachedActor && CurrentStamina >= 2 && !bIsStaminaDepleted) // isGrip�� true�̰�, AttachedActor�� ��ȿ���� Ȯ��
//	{
//		// ���͸� ���� ��ġ���� �и�
//		AttachedActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
//
//		// ��Ʈ ������Ʈ�� UPrimitiveComponent�� ĳ����
//		UPrimitiveComponent* RootPrimitiveComponent = Cast<UPrimitiveComponent>(AttachedActor->GetRootComponent());
//
//		if (RootPrimitiveComponent && RootPrimitiveComponent->IsSimulatingPhysics())
//		{
//			// ������ ���� �ʱ� ��ġ�� �ӵ� ����
//			FVector LaunchVelocity = GetActorForwardVector() * 1000.0f; // ������ �ӵ� �� ���� ����
//			RootPrimitiveComponent->AddImpulse(LaunchVelocity, NAME_None, true);
//
//		}
//
//		DecreaseStamina(2.0f);
//
//		GrippableActor->Drop(); // Drop �Լ� ȣ��
//		AttachedActor = nullptr;
//		GrippableActor = nullptr; // AttachedActor, GrippableActor �ʱ�ȭ
//	}
//}

void ABearCharacter::IncreaseMovementSpeed()
{
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (MovementComp && !bIsSpeedBoosted) // �ӵ� ������ �̹� ����� ���°� �ƴ� ��쿡�� ����
	{
		MovementComp->MaxWalkSpeed *= 1.5f;  // �̵� �ӵ� ����
		MovementComp->JumpZVelocity *= 1.5f; // ���� �ӵ� ����
		bIsSpeedBoosted = true;
	}
}

void ABearCharacter::ResetMovementSpeed()
{
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (MovementComp && bIsSpeedBoosted)
	{
		MovementComp->MaxWalkSpeed /= 1.5f;  // �̵� �ӵ� ���� ����
		MovementComp->JumpZVelocity /= 1.5f; // ���� �ӵ� ���� ����
		bIsSpeedBoosted = false;
	}
}
