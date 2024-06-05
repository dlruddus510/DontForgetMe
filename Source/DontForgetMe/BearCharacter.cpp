

#include "BearCharacter.h"

void ABearCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);
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
