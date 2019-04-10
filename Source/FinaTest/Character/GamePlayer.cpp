// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/Weapon.h"
#include "Engine.h"
// Sets default values
AGamePlayer::AGamePlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//控制相机手臂
	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	CameraArm->SetupAttachment(RootComponent);
	CameraArm->TargetArmLength = 600.f;
	CameraArm->SocketOffset = (FVector(0, 300.f, 200.f));
	CameraArm->RelativeRotation = (FRotator(0, 0, 0));
	CameraArm->bEnableCameraLag = true;
	CameraArm->CameraLagSpeed = 5.f;
	CameraArm->bAbsoluteRotation = true;	//角色的方向改变不会导致摄像头旋转
	CameraArm->bDoCollisionTest = false;

	//控制摄像头
	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	MainCamera->SetupAttachment(CameraArm);

	//运动控制
	IsJumpstart = false;
	JumpPressedTime = 0.f;
	JumpMinHight = 600.f;
	JumpMaxHight = 1000.f;
	GetCharacterMovement()->bOrientRotationToMovement = true;	//角色的正方向会与移动方向相同
	GetCharacterMovement()->JumpZVelocity = JumpMinHight;
	GetCharacterMovement()->GravityScale = 1.5f;
	GetCharacterMovement()->AirControl = 0.f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;

	//角色属性初始化
	Health = 3;
	Coin = 0;
	AttackTime = 1.2f;

	static ConstructorHelpers::FObjectFinder<UBlueprint>Sword(TEXT("/Game/Blueprint/Sword_BP"));
	if (Sword.Object)
	{
		WeaponBP = (UClass*)Sword.Object->GeneratedClass;
	}

	
}

// Called when the game starts or when spawned
void AGamePlayer::BeginPlay()
{
	Super::BeginPlay();
	
	AttachWeapon();
}

// Called every frame
void AGamePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (IsJumpstart)
	{
		JumpPressedTime += DeltaTime;
		if (JumpPressedTime > 0.1f)
			jumploop();
	}// 实现长按高跳 轻按轻跳
	//GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Blue, FString::SanitizeFloat(JumpPressedTime));

}

// Called to bind functionality to input
void AGamePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AGamePlayer::JumpStart);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AGamePlayer::JumpEnd);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AGamePlayer::Attack);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AGamePlayer::CrouchStart);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AGamePlayer::CrouchEnd);
	PlayerInputComponent->BindAction("SpeedUp", IE_Pressed, this, &AGamePlayer::SpeedUp);
	PlayerInputComponent->BindAction("SpeedUp", IE_Released, this, &AGamePlayer::SpeedDown);

	PlayerInputComponent->BindAxis("MoveRight", this, &AGamePlayer::MoveRight);

}

void AGamePlayer::SpeedUp()
{
		GetCharacterMovement()->MaxWalkSpeed = 900.f;
}

void AGamePlayer::SpeedDown()
{
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void AGamePlayer::AttachWeapon()
{
	if (!WeaponInstance)
	{
		UWorld * const World = GetWorld();
		WeaponInstance = World->SpawnActor<AWeapon>(WeaponBP);	//创建一个weapon实例

																//将weapon附着在人物的手上的插槽
		WeaponInstance->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponInstance->AttachPoint);
	}
}

void AGamePlayer::CrouchStart()
{
	if (!GetCharacterMovement()->IsFalling() && !IsAttacked)
	{
		IsCrouch = true;
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Blue, TEXT("Crouch!"));
	}
}

void AGamePlayer::CrouchEnd()
{
	IsCrouch = false;
}

void AGamePlayer::MoveRight(float val)
{
	if (!IsCrouch)
	{
		AddMovementInput(FVector(0, 1, 0), val);
	}
}

//跳跃
void AGamePlayer::JumpEnd()
{
	if (IsJumpstart)
	{
		JumpPressedTime = 0.f;
		IsJumpstart = false;
	}

}
//跳跃中时长会不一样，有一个loop来与跳跃时间匹配
void AGamePlayer::jumploop()
{
	LaunchCharacter(FVector(0, 0, JumpMaxHight - JumpMinHight), false, true);
	if (JumpPressedTime >= 0.4f)	//最大跳0.4秒
	{
		IsJumpstart = false;
		JumpPressedTime = 0.f;
	}
}

void AGamePlayer::JumpStart()
{
	if (CanJump() && !IsCrouch) {
		LaunchCharacter(FVector(0, 0, JumpMinHight), false, false);
		IsJumpstart = true;
	}
}

void AGamePlayer::Attack()
{
	if (!IsAttacked && !GetCharacterMovement()->IsFalling())
	{
		IsAttacked = true;
		GetWorld()->GetTimerManager().SetTimer(FuzeTimerHandle, this, &AGamePlayer::EndAttack, AttackTime, false);	//延时两秒
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Blue, TEXT("ATTACK!"));

	}

}

void AGamePlayer::EndAttack()
{
	IsAttacked = false;
	GetWorld()->GetTimerManager().ClearTimer(FuzeTimerHandle);
	GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Blue, TEXT("EndAttack!"));

}


