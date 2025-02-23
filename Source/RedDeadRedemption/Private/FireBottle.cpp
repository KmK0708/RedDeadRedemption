// Fill out your copyright notice in the Description page of Project Settings.


#include "FireBottle.h"
#include "Enemy.h"
#include "FireBottleFloor.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFireBottle::AFireBottle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("boxComp"));
	SetRootComponent(RootComponent);
	boxComp->SetBoxExtent(FVector(6.0f, 6.0f, 20.0f));

	bottleComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("bottleComp"));
	bottleComp->SetupAttachment(boxComp);
	bottleComp->SetRelativeScale3D(FVector(1.0f));
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Game/Asset/prop/Bottle.Bottle'"));
	if(tempMesh.Succeeded())
	{
		bottleComp->SetStaticMesh(tempMesh.Object);

		bottleComp->SetRelativeLocation(FVector(0, 0, -20.0f));
	}

	fireComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("fireComp"));
	fireComp->SetupAttachment(bottleComp);
	fireComp->SetRelativeScale3D(FVector(1.0f));
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempFireMesh(TEXT("/Script/Engine.StaticMesh'/Game/Asset/prop/bomb_fire.bomb_fire'"));
	if(tempFireMesh.Succeeded())
	{
		fireComp->SetStaticMesh(tempFireMesh.Object);

		fireComp->SetRelativeLocation(FVector(0, 0, 43.0f));
	}

	movementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("movementComp"));
	movementComp->SetUpdatedComponent(boxComp);
	movementComp->InitialSpeed = 1000.0f;
	movementComp->MaxSpeed = 1000.0f;
	movementComp->bShouldBounce = false;
	movementComp->SetVelocityInLocalSpace(FVector(1.0f, 0, 1.0f));
}

// Called when the game starts or when spawned
void AFireBottle::BeginPlay()
{
	Super::BeginPlay();

	floor = Cast<AFireBottleFloor>(UGameplayStatics::GetActorOfClass(GetWorld(), effectFloor));
}

// Called every frame
void AFireBottle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Explode();
}

void AFireBottle::Explode()
{
	TArray<FOverlapResult> hitsInfo;
	FVector centerLoc = GetActorLocation();
	FQuat centerRot = GetActorRotation().Quaternion();
	FCollisionObjectQueryParams params;
	params.AddObjectTypesToQuery(ECC_GameTraceChannel3);
	params.AddObjectTypesToQuery(ECC_GameTraceChannel4);
	params.AddObjectTypesToQuery(ECC_WorldStatic);

	FCollisionShape checkShape = FCollisionShape::MakeSphere(30);
	GetWorld()->OverlapMultiByObjectType(hitsInfo, centerLoc, centerRot, params, checkShape);
	for(FOverlapResult hitInfo : hitsInfo)
	{
		bool bHit = hitInfo.bBlockingHit;

		if(bHit)
		{
			auto enemy = Cast<AEnemy>(hitInfo.GetActor());
			if(enemy == nullptr)
			{
				FVector loc = FVector(GetActorLocation());
				FRotator rot = FRotator(0, 0, 0);
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), explosionImpactFactory, loc);
				GetWorld()->SpawnActor<AFireBottleFloor>(effectFloor, loc, rot);
			}
			else
			{
				FVector eneLoc = FVector(enemy->GetActorLocation().X, enemy->GetActorLocation().Y, enemy->GetActorLocation().Z-88.0f);
				FRotator eneRot = FRotator(0, 0, 0);
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), explosionImpactFactory, eneLoc);
				GetWorld()->SpawnActor<AFireBottleFloor>(effectFloor, eneLoc, eneRot);
			}
			this->Destroy();
		}
	}
	DrawDebugSphere(GetWorld(), centerLoc, 30, 1, FColor::Yellow, false, 0.5);
}

//UKismetSystemLibrary::SphereTraceMulti();

//반경 5미터 이내에 있는 모든 AEnemy엑터들을 감지한다
//감지된 에너미들의 정보를 담을 변수의 배열
//TArray<FOverlapResult> enemiesInfo;
//FVector centerLoc = GetActorLocation() + GetActorUpVector() * 700;
//FQuat centerRot = GetActorRotation().Quaternion();
//FCollisionObjectQueryParams params = FCollisionObjectQueryParams(ECC_GameTraceChannel2);
//FCollisionShape checkShape = FCollisionShape::MakeSphere(500);

//GetWorld()->OverlapMultiByObjectType(enemiesInfo, centerLoc, centerRot, params, checkShape);

//체크된 모든 에너미의 이름을 체크한다
//for (FOverlapResult enemyInfo : enemiesInfo)
//{
//	UE_LOG(LogTemp, Warning, TEXT("Hited : %s"), *enemyInfo.GetActor()->GetName());

//	enemyInfo.GetActor()->Destroy();
//}

//DrawDebugSphere(GetWorld(), centerLoc, 500, 20, FColor::Yellow, false, 2);