// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class REDDEADREDEMPTION_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();
	// �� �޽�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemySettings")
		class USkeletalMeshComponent* EnemyMesh;


	// �� �޴� ����
	UFUNCTION(BlueprintCallable, Category = "EnemySettings")
		void OnMyTakeDamage(float Damage);

	
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
public:
	// �� AI ���� ������Ʈ Ŭ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FMSComponent")
		class UEnemyFSM* myEnemyFSM;

	// �Ѿ� ������ �̺�Ʈ
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bBFromSweep, const FHitResult& SweepResult);

	// �� �޽�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnemySettings")
		class USkeletalMeshComponent* GunMeshComp;
	// ������ �޽�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnemySettings")
		class UStaticMeshComponent* RevolverMeshComp;
};
