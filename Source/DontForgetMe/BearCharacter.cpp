// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFramework/CharacterMovementComponent.h"
#include "BearCharacter.h"


//잡고 던지기
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
	if (!AttachedActor) // 잡고 있지 않다면
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
	else // 이미 잡고 있다면
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
//	if (!AttachedActor) // 잡고 있지 않다면
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
//	else // 이미 잡고 있다면
//
//	{
//		GrippableActor = Cast<IInterfaceGrip>(AttachedActor);
//		if (GrippableActor)
//		{
//			FRotator NewRotation = FRotator(0.f, 0.f, 0.f);
//			FVector DropLocation = GetForwardLocation(150.0f); // 기본 놓는 위치
//
//			FVector CurrentScale = AttachedActor->GetActorScale3D();
//
//			// 스케일을 2배로 설정합니다.
//			FVector NewScale = CurrentScale * 2.0f;
//
//			// 변경된 스케일을 액터에 적용합니다.
//			AttachedActor->SetActorScale3D(NewScale);
//
//			AttachedActor->GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
//
//			// 앞에 오브젝트가 있는지 확인
//			FVector StartLocation = GetForwardLocation(1.0f);
//			FVector EndLocation = GetForwardLocation(50.0f); // 조금 더 앞을 확인
//
//			if (CheckForObjectsInChannel(StartLocation, EndLocation, ECC_GameTraceChannel1, LocalCollisionShape))
//			{
//				// 앞에 오브젝트가 있으면, 그 오브젝트의 중심으로 위치 조정
//				FVector HitLocation = OutHit.ImpactPoint;
//				float ObjectHeight = OutHit.GetActor()->GetSimpleCollisionHalfHeight() * 4.0f; // 해당 오브젝트의 전체 높이 계산
//				FVector CenterLocation = OutHit.GetActor()->GetActorLocation(); // 충돌한 오브젝트의 중심 위치
//
//				NewRotation = OutHit.GetActor()->GetActorRotation();
//
//				// 놓는 물체의 밑바닥이 오브젝트 상단에 오도록 Z 값 조정
//				DropLocation.Z = HitLocation.Z + ObjectHeight + AttachedActor->GetSimpleCollisionHalfHeight();
//				// 충돌한 오브젝트의 중심으로 X와 Y 값을 조정
//				DropLocation.X = CenterLocation.X;
//				DropLocation.Y = CenterLocation.Y;
//			}
//
//			// Detach하고 위치 조정
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
		// AttachedActor의 복사본을 생성합니다.
		AActor* ClonedActor = GetWorld()->SpawnActor<AActor>(AttachedActor->GetClass(), AttachedActor->GetActorLocation(), AttachedActor->GetActorRotation());

		// 복사된 액터에 물리적 속성 설정
		UPrimitiveComponent* RootPrimitiveComponent = Cast<UPrimitiveComponent>(ClonedActor->GetRootComponent());
		if (RootPrimitiveComponent)
		{
			// 포물선을 그리며 날아갈 수 있도록 초기 충격력을 추가합니다.
			FVector LaunchVelocity = GetActorForwardVector() * 1000.0f;
			LaunchVelocity.Z += 500.0f; // 수직 방향으로 추가 속도를 부여합니다.
			RootPrimitiveComponent->AddImpulse(LaunchVelocity, NAME_None, true);

			RootPrimitiveComponent->SetSimulatePhysics(true);
		}

		// 스태미너 감소
		DecreaseStamina(2.0f);

		// 기존에 들고 있던 액터를 삭제합니다.
		AttachedActor->Destroy();

		// 참조 초기화
		AttachedActor = nullptr;
		GrippableActor = nullptr;
	}
}


//void ABearCharacter::Throw()
//{
//	if (AttachedActor && CurrentStamina >= 2 && !bIsStaminaDepleted) // isGrip이 true이고, AttachedActor가 유효한지 확인
//	{
//		// 액터를 현재 위치에서 분리
//		AttachedActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
//
//		// 루트 컴포넌트를 UPrimitiveComponent로 캐스팅
//		UPrimitiveComponent* RootPrimitiveComponent = Cast<UPrimitiveComponent>(AttachedActor->GetRootComponent());
//
//		if (RootPrimitiveComponent && RootPrimitiveComponent->IsSimulatingPhysics())
//		{
//			// 던지기 위한 초기 위치와 속도 설정
//			FVector LaunchVelocity = GetActorForwardVector() * 1000.0f; // 던지는 속도 및 방향 설정
//			RootPrimitiveComponent->AddImpulse(LaunchVelocity, NAME_None, true);
//
//		}
//
//		DecreaseStamina(2.0f);
//
//		GrippableActor->Drop(); // Drop 함수 호출
//		AttachedActor = nullptr;
//		GrippableActor = nullptr; // AttachedActor, GrippableActor 초기화
//	}
//}

void ABearCharacter::IncreaseMovementSpeed()
{
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (MovementComp && !bIsSpeedBoosted) // 속도 증가가 이미 적용된 상태가 아닌 경우에만 실행
	{
		MovementComp->MaxWalkSpeed *= 1.5f;  // 이동 속도 증가
		MovementComp->JumpZVelocity *= 1.5f; // 점프 속도 증가
		bIsSpeedBoosted = true;
	}
}

void ABearCharacter::ResetMovementSpeed()
{
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (MovementComp && bIsSpeedBoosted)
	{
		MovementComp->MaxWalkSpeed /= 1.5f;  // 이동 속도 원상 복귀
		MovementComp->JumpZVelocity /= 1.5f; // 점프 속도 원상 복귀
		bIsSpeedBoosted = false;
	}
}
