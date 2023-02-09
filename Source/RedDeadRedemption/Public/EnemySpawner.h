// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy.h"
#include "RedPlayer.h"
#include "EnemySpawner.generated.h"

UCLASS()
class REDDEADREDEMPTION_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
	// enemy class and a TimerHandle for the spawn timer.
	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<class AEnemy> EnemyClass;

	// �÷��̾�
	UPROPERTY(EditAnywhere, Category = "Spawning")
		class ARedPlayer* Player;

	// �ִ� �� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
		int32 MaxEnemySpawn = 5;

	// ���� ������ �� ��
	UPROPERTY(EditAnywhere, Category = "Spawning")
		int32 EnemiesSpawned = 0;

	UPROPERTY(EditAnywhere, Category = "Spawning")
		float SpawnDelay = 2.0f;

	// �÷��̾� ���� ����
	UPROPERTY(EditAnywhere, Category = "Spawning")
		float PlayerDetectionRange = 2000.0f;

	// �÷��̾� ���� ����
	UPROPERTY(EditAnywhere, Category = "Spawning")
		float PlayerSpawnRange = 150.0f;

	void CheckPlayerDistance();

	FTimerHandle SpawnTimer;

private:
	// A timer handle for the spawn timer.
	int EnemyCount;

	// A function to spawn an enemy.
void SpawnEnemy();
//	void OnEnemyDeath(UEnemyFSM* Enemy);
	
};