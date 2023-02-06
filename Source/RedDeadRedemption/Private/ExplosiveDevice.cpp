// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosiveDevice.h"
#include "Enemy.h"
#include "EnemyFSM.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"



// Sets default values
AExplosiveDevice::AExplosiveDevice()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(BoxComp);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Radius"));
	SphereComp->SetupAttachment(RootComponent);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AExplosiveDevice::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExplosiveDevice::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExplosiveDevice::Explosion()
{
	// ���� ī��Ʈ�� 0�� �ƴ϶��
	if (ExplosionCount > 0)
	{
		// ���� ī��Ʈ�� 1�� ����
		ExplosionCount -= 1;
	}
	// ���� ī��Ʈ�� 0�̶��
	else
	{
		// ���� ����Ʈ ����
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation(), FRotator::ZeroRotator, FVector(1.0f), true);

		//// ���� ���� ���� ������ ã�Ƽ�
		TArray<AActor*> OverlapActors;
		SphereComp->GetOverlappingActors(OverlapActors);

		// ������ ���¸� �������� ����
		for (auto OverlapActor : OverlapActors)
		{
			Enemy = Cast<AEnemy>(OverlapActor);
			if (Enemy)
			{
				Enemy->myEnemyFSM->mState = EEnemyState::Dead;
			}
		}

		// ���� ��ġ ����
		Destroy();
	}
}

