// Copyright (c) 2020 Tension Graphics AB

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class PROJECTROCKET_API AProjectile : public AActor
{
	//UPROPERTY( replicated )
	
	GENERATED_BODY()

public:

	// Sets default values for this actor's properties
	AProjectile();

	// Action functions
	void ShootInDirection(const FVector& ShootDirection);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Component instances
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Projectile)
		class USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, Category = SkeletalMesh)
		USkeletalMeshComponent* SkeletalMeshComponent;

	// To be deleted
	UPROPERTY(VisibleAnywhere, Category = SkeletalMesh)
		UStaticMeshComponent* StaticMeshComponent;
	//

	UPROPERTY(VisibleAnywhere, Category = Movement)
		class UProjectileMovementComponent* ProjectileMovementComponent;

	// Launch variable
	FVector LaunchDirection;
	// Force variables
	UPROPERTY(EditAnywhere, Category = Knockback)
	float Radius = 500.0f;
	UPROPERTY(EditAnywhere, Category = Knockback)
	float KnockbackForce = 3400.0f;
	UPROPERTY(EditAnywhere, Category = Knockback, meta = (ToolTip = "When SelfForce is set to 1, SelfForce is equal to KnockbackForce.\n Force result will only affect projectile owner."))
	float SelfForceMultiplier = 0.5;
	UPROPERTY(EditAnywhere, meta = (ToolTip = "Force scale when in air compared to on ground"))
	float FallingForceMultiplier = 0.5;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;
};
