// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GamePlayer.generated.h"

UCLASS()
class FINATEST_API AGamePlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGamePlayer();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraComp")
		class UCameraComponent* MainCamera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraComp")
		class USpringArmComponent* CameraArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		TSubclassOf<class AWeapon> WeaponBP;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	//�������
	void MoveRight(float val);
	void Attack();
	void JumpEnd();
	void JumpStart();
	void jumploop();
	void AttachWeapon();
	void CrouchStart();
	void CrouchEnd();
	void SpeedUp();
	void SpeedDown();
	float JumpPressedTime;
	bool IsJumpstart;
	class AWeapon *WeaponInstance;	//ʵ����



	FTimerHandle FuzeTimerHandle;
	UFUNCTION()
		void EndAttack();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//�ƶ��������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementComp")
		float JumpMinHight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementComp")
		float JumpMaxHight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MovementComp")
		bool IsCrouch;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementComp")
		bool IsAttacked;
		//��ɫ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RoleAttriBute")
		float Health;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "RoleAttriBute")
		float Coin;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "RoleAttriBute")
		float AttackTime;



	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
