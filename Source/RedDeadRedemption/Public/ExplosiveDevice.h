// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosiveDevice.generated.h"

UCLASS()
class REDDEADREDEMPTION_API AExplosiveDevice : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosiveDevice();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// �ڽ� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosive Settings")
		class UBoxComponent* BoxComp;

	// ���Ǿ� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosive Settings")
		class USphereComponent* SphereComp;

	// �޽� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosive Settings")
		class UStaticMeshComponent* MeshComp;

	// ���� ī��Ʈ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosive Settings")
		float ExplosionCount = 0;

	// ���� ���
	UFUNCTION(BlueprintCallable)
		void Explosion();

	// ���� ȿ��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosive Settings")
		class UParticleSystem* ExplosionEffect;

	class UEnemyFSM* EnemyFSM;
	
	class AEnemy* Enemy;
};
