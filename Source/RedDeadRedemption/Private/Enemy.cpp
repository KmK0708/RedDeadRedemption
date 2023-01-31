// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include <Components/SkeletalMeshComponent.h>
#include "EnemyFSM.h"
#include "EnemyBullet.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	

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
	GunMeshComp->SetupAttachment(GetMesh());
	// �������� ������ �о ������Ʈ�� �����Ѵ�.
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/PolygonWestern/Meshes/Weapons/SK_Wep_Rifle_01.SK_Wep_Rifle_01'"));
	// ���� ������ �����ߴٸ�
	if (TempGunMesh.Succeeded())
	{
		// �������� �޽��� GunMeshComp�� �����Ѵ�.
		GunMeshComp->SetSkeletalMesh(TempGunMesh.Object);
		// transform�� �����Ѵ�.
		GunMeshComp->SetRelativeLocationAndRotation(FVector(0.0f, 60.0f, 140.0f), FRotator(0.0f, 0.0f, 0.0f));
		// scale ����
		GunMeshComp->SetRelativeScale3D(FVector(1.0f));
	}


	// enemy FSM ������Ʈ �߰�
	myEnemyFSM = CreateDefaultSubobject<UEnemyFSM>(TEXT("EnemyFSM"));

	GetCharacterMovement()->bOrientRotationToMovement = true;
}


void AEnemy::OnMyTakeDamage(float Damage)
{

	//EnemyHealth -= Damage;

	//if (EnemyHealth <= 0.0f)
	//{
	//	Destroy();
	//}
	
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

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
	if (OtherActor->IsA<ABullet>())
	{
		// EnemyFSM���� enemy���� �������� �ְ� �ʹ�.
		myEnemyFSM->OnDamageProcess(10.0f);

		UE_LOG(LogTemp, Warning, TEXT("Hit"));
	}
	
}
