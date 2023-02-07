// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include <Components/SkeletalMeshComponent.h>
#include "EnemyFSM.h"
#include "EnemyAnim.h"
#include "EnemyBullet.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerPistolBullet.h"
#include "PlayerRifleBullet.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "Engine/SkeletalMesh.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	// enemy FSM ������Ʈ �߰�
	myEnemyFSM = CreateDefaultSubobject<UEnemyFSM>(TEXT("EnemyFSM"));

	// enemy mesh
	// constructorhelpers�� �̿��ؼ� ĳ������ �޽��� �����´�.
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempEnemyMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/PolygonWestern/Meshes/Characters/SK_Character_Badguy_01.SK_Character_Badguy_01'"));
	// �ҷ����µ� �����ߴٸ�
	if (TempEnemyMesh.Succeeded())
	{
		// �޽��� GetMesh�� �����Ѵ�.
		GetMesh()->SetSkeletalMesh(TempEnemyMesh.Object);
		// transform�� �����Ѵ�.
		GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));
		// scale ����
		GetMesh()->SetRelativeScale3D(FVector(1.0f));
	}

	// �������� ������Ʈ�� ����� �ʹ�.
	GunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));
	GunMeshComp->SetRelativeScale3D(FVector(1.0f));
	GunMeshComp->SetupAttachment(GetMesh(), TEXT("Hand_RSocket"));
	// �������� ������ �о ������Ʈ�� �����Ѵ�.
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/PolygonWestern/Meshes/Weapons/SK_Wep_Rifle_01.SK_Wep_Rifle_01'"));
	// ���� ������ �����ߴٸ�
	if (TempGunMesh.Succeeded())
	{
		// �������� �޽��� GunMeshComp�� �����Ѵ�.
		GunMeshComp->SetSkeletalMesh(TempGunMesh.Object);
		// transform�� �����Ѵ�.
		GunMeshComp->SetRelativeLocationAndRotation(FVector(-10.304565f, -4.293165f, -3.691982f), FRotator(20.551983f, -76.14f, 159.704f));
		
	}

	// ������ ������Ʈ
	RevolverMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RevolverMeshComp"));
	RevolverMeshComp->SetRelativeScale3D(FVector(1.0f));
	RevolverMeshComp->SetupAttachment(GetMesh(), TEXT("Hand_RSocket"));
	// ������ ���� �о ������Ʈ�� ����
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempRevolverMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/PolygonWestern/Meshes/Weapons/SK_Wep_Revolver_01.SK_Wep_Revolver_01'"));
	if (TempRevolverMesh.Succeeded())
	{
		RevolverMeshComp->SetSkeletalMesh(TempRevolverMesh.Object);
		RevolverMeshComp->SetRelativeLocationAndRotation(FVector(-10.304565f, -4.293165f, -3.691982f), FRotator(20.551983f, -76.14f, 159.704f));
	}


	// �ֳʹ̿��� ����Ʈ�������۽�
	ConstructorHelpers::FClassFinder<UAnimInstance> TempAnim(TEXT("/Script/Engine.AnimBlueprint'/Game/Blueprint/Enemy/ABP_Enemy.ABP_Enemy_C'"));
	if (TempAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(TempAnim.Class);
	}

	GetCharacterMovement()->bOrientRotationToMovement = true;
}


void AEnemy::OnDeath()
{
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RevolverMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GunMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	// �ֳʹִ̾� �ٸŽ� ĳ��Ʈ
	enemyAnim = Cast<UEnemyAnim>(GetMesh()->GetAnimInstance());

	// �Ѿ˰��� Overlap�� �����ϱ� ���� �Լ�
	GetMesh()->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapBegin);

}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bBFromSweep, const FHitResult& SweepResult)
{
	// OtherActor�� PlayerPistolBullet �̶��
	if (OtherActor->IsA<APlayerPistolBullet>())
	{
		// EnemyFSM���� enemy���� �������� �ְ� �ʹ�.
		myEnemyFSM->OnDamageProcess(10.0f);

		UE_LOG(LogTemp, Warning, TEXT("Hit"));
	}
	// OtherActor�� PlayerRifleBullet �̶��
	else if (OtherActor->IsA<APlayerRifleBullet>())
	{
		// EnemyFSM���� enemy���� �������� �ְ� �ʹ�.
		myEnemyFSM->OnDamageProcess(20.0f);

		UE_LOG(LogTemp, Warning, TEXT("Hit"));
	}
}

