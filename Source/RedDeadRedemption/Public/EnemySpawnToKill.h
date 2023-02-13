// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawnToKill.generated.h"

UCLASS()
class REDDEADREDEMPTION_API AEnemySpawnToKill : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawnToKill();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
// TArray<AActor> ���� �����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
		TArray<AActor*> EnemyArray;
// ���� ���� ������ ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
		int NumEnemy;
	//�� �迭�� �����ϴ� �Լ�
	UFUNCTION(BlueprintCallable, Category = "Spawning")
		void StoreEnemies();

	// ���ʹ� ���� ����
	void SpawnEnemy();
	
	UPROPERTY()
		bool isSpawn = false;

	UPROPERTY()
		float currentTime = 0.0f;
	
	UPROPERTY()
		float spawnTime = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<class AEnemy> EnemyClass;

	UPROPERTY()
		int enemyCount = 0;

	UPROPERTY()
		int maxCount = 5;
};
