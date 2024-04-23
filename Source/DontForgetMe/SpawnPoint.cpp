// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnPoint.h"

// Sets default values
ASpawnPoint::ASpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}
bool ASpawnPoint::IsSuitableFor(UClass* CharacterClass) const
{
    // �� �Լ��� SpawnPoint�� Ư�� ĳ���� Ŭ������ �������� ���θ� �����մϴ�.
    // ���� ���, Ư�� Ÿ���� ĳ���͸��� �� SpawnPoint�� ����� �� �ִٸ�,
    // ���⿡�� �� ������ üũ�ϰ� true�� false�� ��ȯ�մϴ�.

    // �⺻�����δ� ��� ĳ���Ϳ� �����ϴٰ� �����մϴ�.
    return true;
}