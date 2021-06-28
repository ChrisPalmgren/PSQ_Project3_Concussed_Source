// Copyright (c) 2020 Tension Graphics AB

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RocketPlayer.generated.h"

UCLASS()
class PROJECTROCKET_API ARocketPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARocketPlayer();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool bIsGroundKnockbacked;
	bool AnotherHit;

	UFUNCTION(Server, Reliable)
	void RPCShoot(FRotator rot, FVector loc, FVector dir);
	void RPCShoot_Implementation(FRotator rot, FVector loc, FVector dir);

	UFUNCTION(NetMulticast, Reliable)
	void RPCShootMulti(FRotator rot, FVector loc, FVector dir);
	void RPCShootMulti_Implementation(FRotator rot, FVector loc, FVector dir);

	UFUNCTION(NetMulticast, Reliable)
	void RPCGroundSlide(bool bIsSlideDone);
	void RPCGroundSlide_Implementation(bool bIsSlideDone);

	UFUNCTION(Client, Reliable)
	void RPCDirectHit();
	void RPCDirectHit_Implementation();

	void ReleaseMovement(FVector DeathLocation);

	UPROPERTY(Replicated)
	int PlayerPoints;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;

	void Die();

	UFUNCTION(NetMultiCast, Reliable, WithValidation)
	void RPCDieMulti();
	bool RPCDieMulti_Validate();
	void RPCDieMulti_Implementation();


	FTimerHandle DestroyHandle;
	void CallDestroy();

	void RemoveIgnoredProjectile(AActor* pProjectile);

	void DestroyInputComponent();

	UPROPERTY(BlueprintReadOnly)
	bool bCanMove = true;
	bool bPendingStartMove = false;

	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintSpawn();
	void BlueprintSpawn_Implementation();

	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintShoot();
	void BlueprintShoot_Implementation();

	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintConcussion();
	void BlueprintConcussion_Implementation();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	FRotator CamRot;
	FVector CamLoc;
	FVector CamForward;

	// Action functions
	void Shoot();
	void DoGroundKnockbackFriction(float DeltaTime);
	void FireDelay(float DeltaTime);

	// Movement functions
	void MoveForward(float InputValue);
	void MoveRight(float InputValue);
	void LookUpRate(float InputValue);
	void LookRightRate(float InputValue);

protected:
	// Component variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	class UCameraComponent* Camera;

	UPROPERTY(BlueprintReadOnly)
	bool bCanShoot = true;
private:
	UPROPERTY(VisibleAnywhere, Category = Components)
	USceneComponent* ProjectileSpawnPoint;
	UPROPERTY(VisibleAnywhere, Category = Movement)
	class UCharacterMovementComponent* PlayerMovement;

	// ProjectileClass variable
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AProjectile> ProjectileClass;
	UPROPERTY(EditAnywhere, Category = Projectile)
	float FireRate = 1.0f;
	
	float FireRateDelta = 0.0f;
	UPROPERTY(EditAnywhere, Category = "Ground Friction")
	float BrakeTimeMultiplier = 2.0f;
	float InitialFriction;
	float MinFriction;
	float MaxFriction;

	// Movement variable
	UPROPERTY(EditAnywhere, Category = "Gamepad Controller")
	float RotationRate = 10.0f;

	TArray<USkeletalMeshComponent*> SkeleMeshes;

	UPROPERTY(EditAnywhere, Category = Acceleration)
	float MinAcc = 2048.0f;
	UPROPERTY(EditAnywhere)
	float RespawnDelay = 3.0f;
	UPROPERTY(EditAnywhere, Category = Acceleration)
	float MaxAcc = 8000.0f;
	UPROPERTY(EditAnywhere, Category = Acceleration)
	float AccIncrease = 200.0f;

};
