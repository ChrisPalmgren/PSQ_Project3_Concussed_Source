// Copyright (c) 2020 Tension Graphics AB


#include "Projectile.h"

#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "RocketPlayer.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CollisionComponent->InitSphereRadius(5.0f);
	CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
	RootComponent = CollisionComponent;

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(CollisionComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(SkeletalMeshComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
	ProjectileMovementComponent->InitialSpeed = 3000.0f;
	ProjectileMovementComponent->MaxSpeed = 3000.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = false;

	//Replication
	//SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	SetReplicates(true);
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	CollisionComponent->MoveIgnoreActors.Add(GetOwner());
	//CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);
	//CollisionComponent->IgnoreComponentWhenMoving(Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent()), true);
	CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
}

void AProjectile::BeginDestroy()
{
	Super::BeginDestroy();
	ARocketPlayer* pOwningPlayer = Cast<ARocketPlayer>(GetOwner());
	if (pOwningPlayer)
	{
		pOwningPlayer->RemoveIgnoredProjectile((AActor*)this);
	}
}

void AProjectile::ShootInDirection(const FVector& ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
	LaunchDirection = ProjectileMovementComponent->Velocity;
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (GetLocalRole() == ROLE_Authority)
	{

	
		//if (OtherActor == GetOwner())

		if (OtherActor == GetOwner()) return;

		ARocketPlayer* PlayerDirectHit = Cast<ARocketPlayer>(OtherActor);
		if (PlayerDirectHit != nullptr && PlayerDirectHit->GetCharacterMovement()->IsFalling())
		{
			PlayerDirectHit->RPCDirectHit();
		}
	
		TArray<FHitResult> HitArray;
		const FVector Start = this->GetActorLocation();
		const FVector End = Start;

		const FCollisionShape SphereShape = FCollisionShape::MakeSphere(Radius);
		const bool bSweepHit = GetWorld()->SweepMultiByChannel
		(
			HitArray,
			Start,
			End,
			FQuat::Identity,
			ECC_WorldStatic,
			SphereShape
		);

		if (bSweepHit == true)
		{
			int hitIndex = 0;
			for (const FHitResult HitResult : HitArray)
			{
				OtherActor = HitResult.GetActor();

				AActor* SweepActor = Cast<AActor>(OtherActor);
				if (SweepActor != nullptr && SweepActor->GetRootComponent() != nullptr)
				{
					UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(OtherActor->GetRootComponent());
					//if (StaticMesh != nullptr)
						//StaticMesh->AddRadialImpulse(Start, Radius, KnockbackForce, RIF_Linear, true);
				}
				ARocketPlayer* Player = Cast<ARocketPlayer>(OtherActor);
				Player = Cast<ARocketPlayer>(OtherActor);
				if (Player != nullptr)
				{
					Player->bIsGroundKnockbacked = true;
					Player->AnotherHit = true;

					FVector ProjectilePos = this->GetActorLocation();
					FVector PlayerPos = Player->GetActorLocation();
					PlayerPos.Z += 25.0f;
					FVector PlayerLaunchDirection = (PlayerPos - ProjectilePos);
					float PlayerDistance = LaunchDirection.Size();
					PlayerLaunchDirection.Normalize();
					//PlayerLaunchDirection /= PlayerDistance;
					PlayerLaunchDirection *= KnockbackForce;

					FHitResult BetweenHitCheck;
					bool bHitsBetween = GetWorld()->LineTraceSingleByChannel(BetweenHitCheck, ProjectilePos, PlayerPos, ECollisionChannel::ECC_GameTraceChannel4);

					if (Player->GetCharacterMovement()->IsFalling())
					{
						PlayerLaunchDirection *= FallingForceMultiplier;

						if (Player == GetOwner())
						{
							//Player->GetCharacterMovement()->AddRadialImpulse(Start, Radius, KnockbackForce * SelfForceMultiplier, RIF_Linear, true);			
							PlayerLaunchDirection *= SelfForceMultiplier;
							//Player->GetCharacterMovement()->Launch(PlayerLaunchDirection);
							Player->GetCharacterMovement()->Velocity += PlayerLaunchDirection;
						}
						else
						{
							//Player->GetCharacterMovement()->AddRadialImpulse(Start, Radius, KnockbackForce, RIF_Linear, true);
							//Player->GetCharacterMovement()->Launch(PlayerLaunchDirection);
							Player->GetCharacterMovement()->Velocity += PlayerLaunchDirection;
						}	
					}
					else 
					{
						if (Player == GetOwner())
						{
							//Player->GetCharacterMovement()->AddRadialImpulse(Start, Radius, KnockbackForce * SelfForceMultiplier, RIF_Linear, true);			
							PlayerLaunchDirection *= SelfForceMultiplier;
							Player->GetCharacterMovement()->Launch(PlayerLaunchDirection);
						}
						else
						{
							if (BetweenHitCheck.GetActor() == Player)
							{
								//Player->GetCharacterMovement()->AddRadialImpulse(Start, Radius, KnockbackForce, RIF_Linear, true);
								Player->GetCharacterMovement()->Launch(PlayerLaunchDirection);
							}
						}	
					}
				}
			}
		}
	}
	//DrawDebugSphere(GetWorld(), Start, Radius, 50, FColor::Orange, false, 3.0f);
	this->Destroy();
}
