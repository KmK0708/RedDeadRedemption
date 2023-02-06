// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"
#include "Enemy.h"
#include "EnemyFSM.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	// ���� Ÿ�̸Ӹ� ����
	GetWorld()->GetTimerManager().SetTimer(SpawnTimer, this, &AEnemySpawner::SpawnEnemy, SpawnDelay, true);

	Player = Cast<ARedPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckPlayerDistance();
}

void AEnemySpawner::CheckPlayerDistance()
{
	// �÷��̾���� �Ÿ��� ���
	FVector PlayerLocation = Player->GetActorLocation();

	// �÷��̾�� ������ ������ �Ÿ�
	float Distance = FVector::Dist(PlayerLocation, GetActorLocation());

	// ���� �÷��̾ �������� �ݰ� �ȿ� ������ ����
	if (Distance <= PlayerDetectionRange)
	{
		// ���� Ÿ�̸Ӹ� �ѹ��� ����
		GetWorld()->GetTimerManager().SetTimer(SpawnTimer, this, &AEnemySpawner::SpawnEnemy, SpawnDelay, true);
		UE_LOG(LogTemp, Warning, TEXT("Player Detected"));

	}
	else // ������ ����
	{
		// ���� Ÿ�̸Ӹ� ����
		GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
	}

}

void AEnemySpawner::SpawnEnemy()
{
	// ���� �ֳʹ�
	AEnemy* Enemy = GetWorld()->SpawnActor<AEnemy>(EnemyClass, GetActorLocation(), FRotator::ZeroRotator);
	// �ֳʹ� ���� ī��Ʈ ����
	EnemiesSpawned += 1;

	// ������ �ִ� �ֳʹ� ���� �Ѿ��
	if (EnemiesSpawned >= MaxEnemySpawn)
	{
		// ���� Ÿ�̸Ӹ� ����
		GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
	}

}

