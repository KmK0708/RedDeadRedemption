// Fill out your copyright notice in the Description page of Project Settings.


#include "RedPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Bullet.h"
#include "DeadEyeSpawn.h"
#include "Enemy.h"
#include "EnemyFSM.h"
#include "FireBottle.h"
#include "Kismet/GameplayStatics.h"
#include "Horse.h"
#include "PlayerAnim.h"
#include "WeaponWidget.h"
#include "GamePlayWidget.h"
#include "WeaponSet.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameFramework/Controller.h"
#include "RedDeadRedemption/RedDeadRedemptionGameModeBase.h"

// Sets default values
ARedPlayer::ARedPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/PolygonWestern/Meshes/Characters/SK_Character_Cowboy_01.SK_Character_Cowboy_01'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);

		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90.0f), FRotator(0, -90.0f, 0));
	}
	
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("gunMeshComp"));
	gunMeshComp->SetRelativeScale3D(FVector(1));
	gunMeshComp->SetupAttachment(GetMesh(), TEXT("Hand_RSocket"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/PolygonWestern/Meshes/Weapons/SK_Wep_Rifle_01.SK_Wep_Rifle_01'"));
	if (tempGunMesh.Succeeded())
	{
		gunMeshComp->SetSkeletalMesh(tempGunMesh.Object);

		gunMeshComp->SetRelativeLocationAndRotation(FVector(-10.0f, -4.1f, -8.0f), FRotator(2.4f, -70.0f, 162.0f));
	}

	revolMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("revolMeshComp"));
	revolMeshComp->SetRelativeScale3D(FVector(1));
	revolMeshComp->SetupAttachment(GetMesh(), TEXT("Hand_RSocket"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempRevolMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/PolygonWestern/Meshes/Weapons/SK_Wep_Revolver_01.SK_Wep_Revolver_01'"));
	if (tempRevolMesh.Succeeded())
	{
		revolMeshComp->SetSkeletalMesh(tempRevolMesh.Object);

		revolMeshComp->SetRelativeLocationAndRotation(FVector(-11.0f, -5.5f, -6.0f), FRotator(0, -90.0f, 163.0f));
	}

	bottleMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("bottleMeshComp"));
	bottleMeshComp->SetupAttachment(GetMesh(), TEXT("Hand_RSocket"));
	bottleFireMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("bottleFireMeshComp"));
	bottleFireMeshComp->SetupAttachment(bottleMeshComp);

	springComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("springComp"));
	springComp->SetupAttachment(RootComponent);
	springComp->SetRelativeLocation(FVector(0, 40.0f, 100.0f));
	springComp->TargetArmLength = 250.0f;

	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("cameraComp"));
	cameraComp->SetupAttachment(springComp);

	bUseControllerRotationYaw = false;
	springComp->bUsePawnControlRotation = true;
	cameraComp->bUsePawnControlRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;

	ConstructorHelpers::FClassFinder<UAnimInstance> tempAnim(TEXT("/Script/Engine.AnimBlueprint'/Game/Blueprint/player/ABP_Player.ABP_Player_C'"));
	if(tempAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempAnim.Class);
	}
}

// Called when the game starts or when spawned
void ARedPlayer::BeginPlay()
{
	Super::BeginPlay();

	horsePlayer = Cast<AHorse>(UGameplayStatics::GetActorOfClass(GetWorld(), AHorse::StaticClass()));
	
	weapon_UI = CreateWidget<UWeaponWidget>(GetWorld(), weaponWidget);

	fireBottle = Cast<AFireBottle>(UGameplayStatics::GetActorOfClass(GetWorld(), fireBottleFactory));

	playerAnim = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());

	ChooseWeapon(EWeaponState::FIST);

	playerAnim->isTargetOn = false;

	HP = MaxHP;

	RP = MaxRP;

	gm = Cast<ARedDeadRedemptionGameModeBase>(GetWorld()->GetAuthGameMode());
}

// Called every frame
void ARedPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HPRPCharge();

	FTransform trans(GetControlRotation());
	FVector resultDirection = trans.TransformVector(direction);
	resultDirection.Z = 0;
	resultDirection.Normalize();
	AddMovementInput(resultDirection);
	//���� �ʱ�ȭ
	direction = FVector::ZeroVector;
}

// Called to bind functionality to input
void ARedPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &ARedPlayer::Horizontal);
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &ARedPlayer::Vertical);
	PlayerInputComponent->BindAxis(TEXT("Look Up"), this, &ARedPlayer::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn Right"), this, &ARedPlayer::TurnRight);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ARedPlayer::Jumping);
	PlayerInputComponent->BindAction(TEXT("FireBullet"), IE_Pressed, this, &ARedPlayer::FirePressed);
	PlayerInputComponent->BindAction(TEXT("FireBullet"), IE_Released, this, &ARedPlayer::FireReleased);
	PlayerInputComponent->BindAction(TEXT("HorseRide"), IE_Pressed, this, &ARedPlayer::HorseRide);
	PlayerInputComponent->BindAction(TEXT("WeaponChange"), IE_Pressed, this, &ARedPlayer::WeaponChangePress);
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &ARedPlayer::RunPressed);
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Released, this, &ARedPlayer::RunReleased);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &ARedPlayer::CrouchPressed);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Released, this, &ARedPlayer::CrouchReleased);
	PlayerInputComponent->BindAction(TEXT("Target"), IE_Pressed, this, &ARedPlayer::TargetOnPressed);
	PlayerInputComponent->BindAction(TEXT("Target"), IE_Released, this, &ARedPlayer::TargetOnReleased);
	PlayerInputComponent->BindAction(TEXT("DeadEye"), IE_Pressed, this, &ARedPlayer::OnDeadEye);
	PlayerInputComponent->BindAction(TEXT("DeadEye"), IE_Released, this, &ARedPlayer::OffDeadEye);
	PlayerInputComponent->BindAction(TEXT("Interaction"), IE_Pressed, this, &ARedPlayer::OnInteraction);
	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &ARedPlayer::ReloadAmmo);
}

void ARedPlayer::OnDeadEye()
{
	if(armWeapon == EWeaponState::PISTOL || armWeapon == EWeaponState::RIFLE)
	{
		if(bTarget != false && deadCount > 0)
		{
			bDeadEye = true;

			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1);

			deadCount -= 1;
		}
	}
}

void ARedPlayer::OffDeadEye()
{
	DestroyEnemy();

	bDeadEye = false;

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
}

void ARedPlayer::HPRPCharge()
{
	if (HP <= 100)
	{
		HP += 0.05;
	}
	if (RP <= 100)
	{
		RP += 0.05;
	}
}


void ARedPlayer::Horizontal(float value)
{
	direction.Y = value;
}

void ARedPlayer::Vertical(float value)
{
	direction.X = value;
}

void ARedPlayer::LookUp(float value)
{
	AddControllerPitchInput(value);
}

void ARedPlayer::TurnRight(float value)
{
	AddControllerYawInput(value);
}

void ARedPlayer::Jumping()
{
	if(playerAnim->isTargetOn == false)
	{
		Jump();
	}
}

void ARedPlayer::FirePressed()
{
	if(bTarget != false && bDeadEye != true)
	{
		//bFire = true;
		FVector loc = GetActorLocation();
		switch (armWeapon)
		{
		case EWeaponState::FIST:
			FireFist();
			break;

		case EWeaponState::PISTOL:
			if(pistolAmmo > 0)
			{
				FirePistol();
				FireAnim();
				PlaySound(pistolFireSound, loc);
				pistolAmmo -= 1;
			}
			break;

		case EWeaponState::RIFLE:
			if(rifleAmmo > 0)
			{
				FireRifle();
				FireAnim();
				PlaySound(gunFireSound, loc);
				rifleAmmo -= 1;
			}
			break;

		case EWeaponState::FIREBOTTLE:
			FireBottle();
			break;

		default:
			break;
		}
		//bFire = false;
	}
	else if(bTarget != false && bDeadEye != false)
	{
		switch (armWeapon)
		{
		case EWeaponState::PISTOL:
			DeadEyeTarget();
			break;

		case EWeaponState::RIFLE:
			DeadEyeTarget();
			break;

		default:
			break;
		}
	}
}

void ARedPlayer::FireReleased()
{

}

void ARedPlayer::HorseRide()
{	
	if (bPressed != false)
	{
		//�÷��̾� ��Ʈ�ѷ� �ѱ��
		GetWorld()->GetFirstPlayerController()->Possess(horsePlayer);
		SetActorLocation(FVector(horsePlayer->GetActorLocation().X, horsePlayer->GetActorLocation().Y, horsePlayer->GetActorLocation().Z + 20));
		SetActorRotation(horsePlayer->GetActorRotation());
		playerAnim->OnAnim(TEXT("Mount"));
	}
}

void ARedPlayer::HorseUnRide()
{
	playerAnim->OnAnim(TEXT("Dismount"));
}

void ARedPlayer::UnRideAnimEnd()
{
	horsePlayer->UnRide();
}


void ARedPlayer::WeaponChangePress()
{
	if (weapon_UI && false == weapon_UI->IsInViewport() && bGetWeapon != false)
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1);
		weapon_UI->AddToViewport();
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetShowMouseCursor(true);
		GetWorld()->GetFirstPlayerController()->AController::SetIgnoreLookInput(true);
		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(GetWorld()->GetFirstPlayerController(), weapon_UI);
	}
}

void ARedPlayer::RunPressed()
{
	if (playerAnim->isTargetOn == false && RP >= 0)
	{
		GetCharacterMovement()->MaxWalkSpeed = runSpeed;

		RP -= 10.0f;

		FTimerHandle runTimer;
		GetWorld()->GetTimerManager().SetTimer(runTimer, this, &ARedPlayer::RunTime, 5.0f, false);
	}
}

void ARedPlayer::RunTime()
{
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
}


void ARedPlayer::RunReleased()
{
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
}

void ARedPlayer::TargetOnPressed()
{
	playerAnim->isTargetOn = true;
	bTarget = playerAnim->isTargetOn;
	bUseControllerRotationYaw = true;
}

void ARedPlayer::TargetOnReleased()
{
	playerAnim->isTargetOn = false;
	bTarget = playerAnim->isTargetOn;
	bUseControllerRotationYaw = false;
}

void ARedPlayer::CrouchPressed()
{
	GetCharacterMovement()->MaxWalkSpeedCrouched = crouchSpeed;
	Crouch();
}

void ARedPlayer::CrouchReleased()
{
	GetCharacterMovement()->MaxWalkSpeedCrouched = walkSpeed;
	UnCrouch();
}

void ARedPlayer::ChangeFist()
{
	if(bIsRide != true)
	{
		if (weapon_UI != nullptr && true == weapon_UI->IsInViewport())
		{
			weapon_UI->RemoveFromParent();
			ControllerWidget();
			ChooseWeapon(EWeaponState::FIST);
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
		}
	}
}

void ARedPlayer::ChangeRifle()
{
	if(bIsRide != true)
	{
		if (weapon_UI != nullptr && true == weapon_UI->IsInViewport())
		{
			weapon_UI->RemoveFromParent();
			ControllerWidget();
			ChooseWeapon(EWeaponState::RIFLE);
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
		}
	}
}

void ARedPlayer::ChangePistol()
{
	if (weapon_UI != nullptr && true == weapon_UI->IsInViewport())
	{
		weapon_UI->RemoveFromParent();
		ControllerWidget();
		ChooseWeapon(EWeaponState::PISTOL);
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
	}
}

void ARedPlayer::ChangeBottle()
{
	if(weapon_UI != nullptr && true == weapon_UI->IsInViewport())
	{
		weapon_UI->RemoveFromParent();
		ControllerWidget();
		ChooseWeapon(EWeaponState::FIREBOTTLE);
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
	}
}

void ARedPlayer::ControllerWidget()
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetShowMouseCursor(false);
	GetWorld()->GetFirstPlayerController()->AController::SetIgnoreLookInput(false);
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(GetWorld()->GetFirstPlayerController());
}

void ARedPlayer::ChooseWeapon(EWeaponState val)
{
	switch (val)
	{
	case EWeaponState::FIST:
			gunMeshComp->SetVisibility(false);
			revolMeshComp->SetVisibility(false);
			bottleMeshComp->SetVisibility(false);
			bottleFireMeshComp->SetVisibility(false);
			armWeapon = val;
			playerAnim->state = armWeapon;
		break;

	case EWeaponState::PISTOL:
			gunMeshComp->SetVisibility(false);
			revolMeshComp->SetVisibility(true);
			bottleMeshComp->SetVisibility(false);
			bottleFireMeshComp->SetVisibility(false);
			armWeapon = val;
			playerAnim->state = armWeapon;
		break;

	case EWeaponState::RIFLE:
			gunMeshComp->SetVisibility(true);
			revolMeshComp->SetVisibility(false);
			bottleMeshComp->SetVisibility(false);
			bottleFireMeshComp->SetVisibility(false);
			armWeapon = val;
			playerAnim->state = armWeapon;
		break;

	case EWeaponState::FIREBOTTLE:
			gunMeshComp->SetVisibility(false);
			revolMeshComp->SetVisibility(false);
			bottleMeshComp->SetVisibility(true);
			bottleFireMeshComp->SetVisibility(true);
			armWeapon = val;
			playerAnim->state = armWeapon;
	default:
		break;
	}
}

void ARedPlayer::DeadEyeTarget()
{
	FHitResult deadInfo;
	FVector start = cameraComp->GetComponentLocation();
	FVector end = start + cameraComp->GetForwardVector() * 200000.0f;
	FCollisionObjectQueryParams objParams;
	objParams.AddObjectTypesToQuery(ECC_GameTraceChannel3);

	bool bEnemyHit = GetWorld()->LineTraceSingleByObjectType(deadInfo, start, end, objParams);
	if(bEnemyHit)
	{
		auto enemy = Cast<AEnemy>(deadInfo.GetActor());
		enemies.Add(enemy);
		FVector loc = (deadInfo.GetActor()->GetActorLocation());
		FVector finLoc = loc + FVector(0, 0, 70.0f);
		GetWorld()->SpawnActor<ADeadEyeSpawn>(deadFactory, finLoc, FRotator::ZeroRotator);
	}
}

void ARedPlayer::DestroyEnemy()
{
	for (int i = 0; i < enemies.Num(); i++)
	{
		gm->deadeyes[i]->Destroy();

		UEnemyFSM* fsm = Cast<UEnemyFSM>(enemies[i]->GetDefaultSubobjectByName(TEXT("EnemyFSM")));

		fsm->OnDamageProcess(100);
	}

	enemies.Empty();

	gm->deadeyes.Empty();
}

void ARedPlayer::OnInteraction()
{
	FHitResult hitInfo;
	FVector startLoc = cameraComp->GetComponentLocation();
	FVector endLoc = startLoc + cameraComp->GetForwardVector() * 500.0f;
	FCollisionObjectQueryParams objParams;
	objParams.AddObjectTypesToQuery(ECC_GameTraceChannel5);

	bool bBoxHit = GetWorld()->LineTraceSingleByObjectType(hitInfo, startLoc, endLoc, objParams);
	if(bBoxHit)
	{
		auto weapon = Cast<AWeaponSet>(hitInfo.GetActor());
		if(weapon != nullptr)
		{
			weapon->DestroySelf();

			bGetWeapon = true;

			AmmoSet();
		}
	}
}

void ARedPlayer::AmmoSet()
{
	holdBotlleAmmo = 30;
	holdPistolAmmo = 120;
	holdRifleAmmo = 100;
}

void ARedPlayer::ReloadAmmo()
{
	if(playerAnim->isCrouching !=true)
	{
		playerAnim->OnShootAnim(TEXT("Reload"));
	}
	else
	{
		playerAnim->OnShootAnim(TEXT("CrouchReload"));
	}
}

void ARedPlayer::FireAnim()
{
	if(playerAnim->isCrouching != true)
	{
		playerAnim->OnShootAnim(TEXT("Fire"));
	}
	else
	{
		playerAnim->OnShootAnim(TEXT("CrouchFire"));
	}
}

void ARedPlayer::FirePistol()
{
	FHitResult hitInfo;
	FVector start = cameraComp->GetComponentLocation();
	FVector end = start + cameraComp->GetForwardVector() * 200000.0f;
	FCollisionObjectQueryParams objParams;
	FCollisionObjectQueryParams objParams1;
	objParams.AddObjectTypesToQuery(ECC_GameTraceChannel3);
	objParams1.AddObjectTypesToQuery(ECC_GameTraceChannel4);

	bool bEnemyHit = GetWorld()->LineTraceSingleByObjectType(hitInfo, start, end, objParams);
	if (bEnemyHit)
	{
		FTransform trans(hitInfo.ImpactPoint);
		FVector loc(hitInfo.ImpactPoint);
		SpawnEmitter(pistolEnemyImpactFactory, trans);
		PlaySound(enemyImpactSound, loc);

		auto enemy = Cast<AEnemy>(hitInfo.GetActor());
		if (enemy != nullptr)
		{
			UEnemyFSM* fsm = Cast<UEnemyFSM>(enemy->GetDefaultSubobjectByName(TEXT("EnemyFSM")));

			fsm->OnDamageProcess(10);
		}
	}

	bool bGround = GetWorld()->LineTraceSingleByObjectType(hitInfo, start, end, objParams1);
	if (bGround)
	{
		FTransform trans(hitInfo.ImpactPoint);
		FVector loc(hitInfo.ImpactPoint);
		SpawnEmitter(GroundImpactFactory, trans);
		PlaySound(groundImpactSound, loc);
	}
}

void ARedPlayer::FireRifle()
{
	FHitResult hitInfo;
	FVector start = cameraComp->GetComponentLocation();
	FVector end = start + cameraComp->GetForwardVector() * 200000.0f;
	FCollisionObjectQueryParams objParams;
	FCollisionObjectQueryParams objParams1;
	//ä��3 - enemy
	objParams.AddObjectTypesToQuery(ECC_GameTraceChannel3);
	//ä��4 - ground
	objParams1.AddObjectTypesToQuery(ECC_GameTraceChannel4);

	bool bEnemyHit = GetWorld()->LineTraceSingleByObjectType(hitInfo, start, end, objParams);
	if(bEnemyHit)
	{
		FTransform trans(hitInfo.ImpactPoint);
		FVector loc(hitInfo.ImpactPoint);
		SpawnEmitter(gunEnemyImpactFactory, trans);
		PlaySound(enemyImpactSound, loc);

		auto enemy = Cast<AEnemy>(hitInfo.GetActor());
		if(enemy != nullptr)
		{
			UEnemyFSM* fsm = Cast<UEnemyFSM>(enemy->GetDefaultSubobjectByName(TEXT("EnemyFSM")));

			fsm->OnDamageProcess(25);
		}
	}

	bool bGroundHit = GetWorld()->LineTraceSingleByObjectType(hitInfo, start, end, objParams1);
	if (bGroundHit)
	{
		FTransform trans(hitInfo.ImpactPoint);
		FVector loc(hitInfo.ImpactPoint);
		SpawnEmitter(GroundImpactFactory, trans);
		PlaySound(groundImpactSound, loc);
	}
}

void ARedPlayer::FireFist()
{
	
}

void ARedPlayer::FireBottle()
{
	if(playerAnim != nullptr)
	{
		playerAnim->OnAnim(TEXT("Throw"));
		GetCharacterMovement()->MaxWalkSpeed = 0.0f;
	}
}

void ARedPlayer::SpawnBottle()
{
	GetWorld()->SpawnActor<AFireBottle>(fireBottleFactory, GetActorLocation() + (GetActorUpVector() * 30.0f) + (GetActorForwardVector() * 70.0f), FRotator(GetControlRotation().Pitch, GetControlRotation().Yaw, 0));
}

void ARedPlayer::SpawnEmitter(UParticleSystem* factory, FTransform transform)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), factory, transform);
}

void ARedPlayer::PlaySound(USoundBase* sound, FVector location)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), sound, location);
}

void ARedPlayer::OnDamage(float damage)
{
	HP -= damage;

	if(HP <= 0)
	{
		playerAnim->OnAnim(TEXT("Dead"));
	}
	else
	{
		HP -= damage;
	}
}

void ARedPlayer::Ride()
{
	horsePlayer->ChangeMesh(false);

	//�÷��̾� �޽� �Ⱥ��̰��ϱ�
	GetMesh()->SetVisibility(false);
	
	//�÷��̾� �ݸ��� ����
	this->SetActorEnableCollision(false);
	
	bIsRide = true;

	if (armWeapon == EWeaponState::FIREBOTTLE)
	{
		horsePlayer->ChooseWeapon(EWeaponArm::FIREBOTTLE);
	}
	else if (armWeapon == EWeaponState::FIST)
	{
		horsePlayer->ChooseWeapon(EWeaponArm::FIST);
	}
	else if (armWeapon == EWeaponState::PISTOL)
	{
		horsePlayer->ChooseWeapon(EWeaponArm::PISTOL);
	}
	else if (armWeapon == EWeaponState::RIFLE)
	{
		horsePlayer->ChooseWeapon(EWeaponArm::RIFLE);
	}

	ChooseWeapon(EWeaponState::FIST);
}