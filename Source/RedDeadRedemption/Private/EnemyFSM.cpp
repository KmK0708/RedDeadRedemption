// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include "Enemy.h"
#include "RedPlayer.h"
#include <Kismet/GameplayStatics.h>
#include <Components/CapsuleComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include	"EnemyAI.h"
#include "PlayerPistolBullet.h"
#include "PlayerRifleBullet.h"
#include "EnemyBullet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "EnemyAnim.h"
#include "EnemySpawnToKill.h"


// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	// ���忡�� ARedPlayer Ÿ�� ã��

	// ARedPlayer Ÿ������ ĳ����
	// ���� ��ü ��������
	me = Cast<AEnemy>(GetOwner());

	// �¾�� ���� ü���� �ִ� ü������ ����
	EnemyHealth = EnemyMaxHealth;

	// AAIController Ÿ������ ĳ����
	AI = Cast<AAIController>(me->GetController());


	
	// �ֳʹ� ��ȯ
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawnToKill::StaticClass(), bpEnemySpawner);
	for (int i = 0; i < bpEnemySpawner.Num(); i++)
	{
		AEnemySpawnToKill* emptySpawner = nullptr;
		enemySpawns.Add(emptySpawner);
	}

	for (int i = 0; i < bpEnemySpawner.Num(); i++)
	{
		auto spawn = Cast<AEnemySpawnToKill>(bpEnemySpawner[i]);
		enemySpawns[i] = spawn;
		if (enemySpawns[i]->NumEnemy == 0)
		{
			enemySpawns[i]->isSpawn = true;
		}
	}
}

// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	{
		target = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn();

		switch (mState)
		{
		case EEnemyState::Idle:
			IdleState();
			break;
		case EEnemyState::Move:
			MoveState();
			break;
		case EEnemyState::Attack:
			AttackState();
			break;
		case EEnemyState::Damage:
			DamageState();
			break;
		case EEnemyState::Dead:
			DeadState();
			break;
		}
	}
}

// ��� ����
void UEnemyFSM::IdleState()
{
	// ���� �÷��̾� �������� ����
		FCollisionShape shape;
		shape.SetSphere(3000.0f);
		// ���� ��ġ
		FVector location = me->GetActorLocation();
		// ���� ��ġ���� �÷��̾� �������� ����
		TArray<FOverlapResult> Overlaps;
		// ���� ��ġ���� �÷��̾� �������� ����
		GetWorld()->OverlapMultiByChannel(Overlaps, location, FQuat::Identity, ECollisionChannel::ECC_Pawn, shape);
		// �÷��̾ ���������� ������

		// �÷��̾ ���� ���� ������
		if (Overlaps.Num() == 0)
		{
			// ���¸� ��� ���·� ����
			mState = EEnemyState::Idle;
			me->enemyAnim->State = mState;
			// �ݺ��� ����
			return;
		}

		for (auto overlap : Overlaps)
		{
			// �÷��̾� Ÿ������ ĳ����
			// �÷��̾ �����ϸ�
			if (auto player = Cast<ARedPlayer>(overlap.GetActor()))
			{
				target = player;
				// �÷��̾ Ÿ������ ����
				// ���¸� �̵� ���·� ����
				mState = EEnemyState::Move;
				me->enemyAnim->State = mState;
				// �ݺ��� ����
				break;
			}
		}
}

// �̵� ����
void UEnemyFSM::MoveState()
{
	// 1. Ÿ�� �������� �ʿ���.
	FVector destination = target->GetActorLocation();
	// 2. ������ �ʿ���.
	FVector direction = destination - me->GetActorLocation();
	// 3. �������� EnemyRunSpeed�ӵ��� �̵��ϰ� �ʹ�.

	me->GetCharacterMovement()->MaxWalkSpeed = EnemyRunSpeed;

	//	me->AddMovementInput(direction.GetSafeNormal());

	AI->MoveToLocation(destination, 300.0f);

	// Ÿ��� ����� ���� ���� ���·� ��ȯ�ϰ� �ʹ�.
	// 1. ���� �Ÿ��� ���� ���� �ȿ� ������..
	if (direction.Size() < AttackRange)
	{
		// 2. ���¸� ���� ���·� ����
		mState = EEnemyState::Attack;
		me->enemyAnim->State = mState;
	}
}

// ���� ����
void UEnemyFSM::AttackState()
{
	// ��ǥ : �����ð� ���� �ѹ��� �����ϰ�ʹ�.
	// 1. �ð��� �귶���ϱ�
	currentTime += GetWorld()->DeltaTimeSeconds;
	// 2. ���ݽð��� ����ߴ��� Ȯ��
	if (currentTime > AttackDelayTime)
	{
		// Enemy.cpp���ִ� OnFire �Լ��� ȣ��
		me->OnFire();
		// 3. ���ݼҸ� ���
		// Enemy���ִ� GunMeshComp�� �̿��ؼ� ����
		FTransform transform = me->GunMeshComp->GetSocketTransform("SK_Wep_Rifle_01_SlideSocket");
		GetWorld()->SpawnActor<AEnemyBullet>(EnemyRifleBulletFactory, transform);

		UGameplayStatics::PlaySoundAtLocation(GetWorld(), me->EnemyAttackSound, me->GetActorLocation(),me->GetActorRotation());
		// 4. ��� �ð� �ʱ�ȭ
		currentTime = 0.0f;

		// ��ǥ : Ÿ���� ���� ������ ����� ���¸� �̵����� ��ȯ�ϰ� �ʹ�.
		// 1. Ÿ����� �Ÿ��� �ʿ��ϴ�.
		float distance = FVector::Distance(target->GetActorLocation(), me->GetActorLocation());
		// 2. ���� �Ÿ��� ���� �������� ũ��
		if (distance > AttackRange)
		{
			// 3. ���¸� �̵� ���·� ����
			mState = EEnemyState::Move;
			me->enemyAnim->State = mState;
		}
		else
		{
			//���� �ȿ������� ��� ����
			me->enemyAnim->bAttackPlay = true;
		}
	}
	// �÷��̾���� �Ÿ� ���
	FVector direction = target->GetActorLocation() - me->GetActorLocation();
	float distance = FVector::Distance(target->GetActorLocation(), me->GetActorLocation());

	// ���� �÷��̾ ���� ���� �ȿ� ������
	if (distance <= AttackRange)
	{
		// ���� �÷��̾ �ٶ� �� �ʿ��� �����̼� ��
		FRotator lookAtRotation = FRotationMatrix::MakeFromX(direction).Rotator();
		// ���� �÷��̾ �ٶ󺸴� �����̼�
		me->SetActorRotation(FMath::Lerp(me->GetActorRotation(), lookAtRotation, 0.1f));
	}
}

// �ǰ� ����
void UEnemyFSM::DamageState()
{
	currentTime += GetWorld()->GetDeltaSeconds();
	// currentTime�� 1�ʰ� ������
	if (currentTime > 1.0f)
	{
		// ���¸� �̵� ���·� ����
		mState = EEnemyState::Move;
		me->enemyAnim->State = mState; // ��� �ð� �ʱ�ȭ
		currentTime = 0.0f;
	}
}

// ��� ����
void UEnemyFSM::DeadState()
{
	currentTime += GetWorld()->GetDeltaSeconds();

		// ���
	if (currentTime > 3.0f)
	{
		for (int i = 0; i < bpEnemySpawner.Num(); i++)
		{
			enemySpawns[i]->NumEnemy--;

			if (enemySpawns[i]->NumEnemy <= 0)
			{
				enemySpawns[i]->isSpawn = true;
			}
		}
	// currentTime�� 1�ʰ� ������ ���
		me->Destroy();
	}
}

void UEnemyFSM::OnDamageProcess(int32 damage)
{
	EnemyHealth -= damage;

	// ü���� 0���ϸ�
	if (EnemyHealth <= 0)
	{
		// ���¸� ��� ���·� ����
		bIsDead = true;
		me->OnDeath();
		mState = EEnemyState::Dead;
		me->enemyAnim->State = mState;
	}
	else
	{
		// ���¸� �ǰ� ���·� ����
		mState = EEnemyState::Damage;
		me->enemyAnim->State = mState;
	}
}

void UEnemyFSM::OnAttackEvent()
{
}

void UEnemyFSM::MinusNumEnemies()
{
	AEnemySpawnToKill* EnemySpawner = Cast<AEnemySpawnToKill>(UGameplayStatics::GetActorOfClass(GetWorld(), AEnemySpawnToKill::StaticClass()));
	// ���� EnemySpawnToKill���� NumEnemy�� ���ҵǸ�
	if (EnemySpawner)
	{
		if (EnemySpawner->NumEnemy <= 0)
		{
			
		}
	}
}
