// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAI.h"
#include "Enemy.h"

void AEnemyAI::AEnemyAIController()
{
}

// tick �Լ�
void AEnemyAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	MoveToLocation(EnemyLocation, -1.0f, false, true, false, true, 0, true);
}

// ���� ������
void AEnemyAI::SetNewRandomTargetLocation()
{
	// ���� ����
	FVector RandomLocation = FMath::VRand();
	// ���� ���� ����
	RandomLocation.X = RandomLocation.X * 1000.0f;
	RandomLocation.Y = RandomLocation.Y * 1000.0f;
	RandomLocation.Z = 0.0f;
	// ���� ��ġ ����
	EnemyLocation = GetPawn()->GetActorLocation() + RandomLocation;
}

// ���� �Ǵ� ���������� ������
void AEnemyAI::MoveToRightOrLeft()
{
	float RandomX = FMath::RandRange(-1000.0f, 1000.0f);
	
	EnemyLocation.X += RandomX;
}