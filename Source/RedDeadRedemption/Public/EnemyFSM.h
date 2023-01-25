// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSM.generated.h"

// ����� ���� ����
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle,
	Move,
	Attack,
	Damage,
	Dead
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REDDEADREDEMPTION_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
		EEnemyState mState = EEnemyState::Idle;

	// ��� ����
	void IdleState();
	// �̵� ����
	void MoveState();
	// ���� ����
	void AttackState();
	// �ǰ� ����
	void DamageState();
	// ��� ����
	void DeadState();

	// ��� �ð�
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FSM")
		float IdleDelayTime = 2.0f;
	// ��� �ð�
	float currentTime = 0.0f;

	// Ÿ��
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSM")
		class ARedPlayer* target;

	// ���� ����
	UPROPERTY()
		class AEnemy* me;

	// ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM")
		float AttackRange = 400.0f;

	// ���� ������
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM")
		float AttackDelayTime = 1.5f;

	// �ǰ� �˸� �̺�Ʈ �Լ�
	void OnDamageProcess();
	

public:
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	//	FVector EnemyLocation;

	//UFUNCTION(BlueprintCallable, Category = "AI")
	//	void SetTargetLocation(FVector newTargetLocation);
	
};
