// Copyright (c) 2020 Tension Graphics AB


#include "RocketPlayer.h"

#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/Unrealnetwork.h"
#include "Projectile.h"
#include "RocketGameModeBase.h"
#include "TimerManager.h"
#include "Engine/Engine.h"

// Sets default values
ARocketPlayer::ARocketPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);
	Camera->bUsePawnControlRotation = true;

	GetCharacterMovement()->GroundFriction = 8.0f;

	/*
	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile Spawnpoint"));
	ProjectileSpawnPoint->SetupAttachment(Camera);
	*/
	//SetReplicates(true);
}

void ARocketPlayer::BlueprintSpawn_Implementation()
{
}

void ARocketPlayer::BlueprintShoot_Implementation()
{
}

void ARocketPlayer::BlueprintConcussion_Implementation()
{
}

// Called when the game starts or when spawned
void ARocketPlayer::BeginPlay()
{
	Super::BeginPlay();

	PlayerPoints = 0;
	bIsGroundKnockbacked = false;
	AnotherHit = false;

	MinFriction = 0.0f;
	MaxFriction = GetCharacterMovement()->GroundFriction;
	InitialFriction = MinFriction;

	GetCharacterMovement()->MaxAcceleration = MinAcc;

	SetInstigator(this);

	GetComponents<USkeletalMeshComponent>(SkeleMeshes);
}

// Called every frame
void ARocketPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bPendingStartMove == false)
	{
		if (ARocketGameModeBase* GameMode = Cast<ARocketGameModeBase>(GetWorld()->GetAuthGameMode()))
		{
			if (GameMode->bGamehasStarted == true)
			{
				if (ARocketPlayerController* RocketController = Cast<ARocketPlayerController>(Controller))
				{
					RocketController->RPCActivateMovement();
				}
			}
		}
	}

	FireDelay(DeltaTime);
	DoGroundKnockbackFriction(DeltaTime);

	float Acceleration = GetCharacterMovement()->MaxAcceleration;

	if (GetCharacterMovement()->IsFalling() && Acceleration < MaxAcc)
	{
		float NewAcc = Acceleration + AccIncrease;
		if (NewAcc > MaxAcc)
		{
			NewAcc = MaxAcc;
		}
		GetCharacterMovement()->MaxAcceleration = NewAcc;
	}
	else if (!GetCharacterMovement()->IsFalling())
	{
		GetCharacterMovement()->MaxAcceleration = MinAcc;
	}
	//GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, FString::SanitizeFloat(Acceleration) );
}

// Called to bind functionality to input
void ARocketPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Shoot"), EInputEvent::IE_Pressed, this, &ARocketPlayer::Shoot);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ARocketPlayer::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ARocketPlayer::MoveRight);
}

void ARocketPlayer::DoGroundKnockbackFriction(float DeltaTime)
{
	if (bIsGroundKnockbacked == true)
	{
		if (AnotherHit == true)
		{
			InitialFriction = MinFriction;
			AnotherHit = false;
		}
		InitialFriction = UKismetMathLibrary::FInterpTo_Constant(InitialFriction, MaxFriction, GetWorld()->GetDeltaSeconds(), BrakeTimeMultiplier);

		if (InitialFriction >= MaxFriction)
		{
			RPCGroundSlide(true);
			InitialFriction = MinFriction;
			bIsGroundKnockbacked = false;
			return;
		}
		else
			RPCGroundSlide(false);
	}
}

void ARocketPlayer::FireDelay(float DeltaTime)
{
	if (bCanShoot == false)
	{
		FireRateDelta += DeltaTime;
		if (FireRateDelta > FireRate)
		{
			bCanShoot = true;
		}
	}
}

// Movement functions
void ARocketPlayer::MoveForward(float InputValue)
{
	if (bCanMove == false || bPendingStartMove == false)
	{
		return;
	}
	AddMovementInput(GetActorForwardVector() * InputValue);
}

void ARocketPlayer::MoveRight(float InputValue)
{
	if (bCanMove == false || bPendingStartMove == false)
	{
		return;
	}
	AddMovementInput(GetActorRightVector() * InputValue);
}

void ARocketPlayer::LookUpRate(float InputValue)
{
	if (bCanMove == false || bPendingStartMove == false)
	{
		return;
	}
	AddControllerPitchInput(InputValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

void ARocketPlayer::LookRightRate(float InputValue)
{
	if (bCanMove == false || bPendingStartMove == false)
	{
		return;
	}
	AddControllerYawInput(InputValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

// Action functions
void ARocketPlayer::Shoot()
{
	if (bCanMove == false || bPendingStartMove == false)
	{
		return;
	}
	//APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	//PlayerController->GetPlayerViewPoint(CamLoc, CamRot);

	if (bCanShoot == true)
	{
		BlueprintShoot();
		CamLoc = Camera->GetComponentTransform().GetLocation();
		CamRot = Camera->GetComponentTransform().GetRotation().Rotator();

		CamForward = Camera->GetForwardVector();

		CamLoc += CamForward * 10.0f;

		RPCShoot(CamRot, CamLoc, CamForward);
		FireRateDelta = 0.0f;
		bCanShoot = false;
	}
}

void ARocketPlayer::RPCShoot_Implementation(FRotator rot, FVector loc, FVector dir)
{
	RPCShootMulti(rot, loc, dir);
}

void ARocketPlayer::RPCShootMulti_Implementation(FRotator rot, FVector loc, FVector dir)
{
	if (ProjectileClass == NULL) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, loc, rot, SpawnParams);
	if (Projectile == nullptr) return;

	GetCapsuleComponent()->MoveIgnoreActors.Add((AActor*)Projectile);

	FVector LaunchDirection = dir;
	Projectile->ShootInDirection(LaunchDirection);
}

void ARocketPlayer::RemoveIgnoredProjectile(AActor* pProjectile)
{
	float fArrayLength = GetCapsuleComponent()->MoveIgnoreActors.Num();
	for (int ignoredActors = 0; ignoredActors < fArrayLength; ++ignoredActors)
	{
		if (GetCapsuleComponent()->MoveIgnoreActors[ignoredActors] == pProjectile)
		{
			GetCapsuleComponent()->MoveIgnoreActors.RemoveAt(ignoredActors, 1, true);
			return;
		}
	}
}

void ARocketPlayer::DestroyInputComponent()
{
	DestroyPlayerInputComponent();
}

void ARocketPlayer::RPCGroundSlide_Implementation(bool bIsSlideDone)
{
	if (bIsSlideDone == true)
		GetCharacterMovement()->GroundFriction = MaxFriction;
	else if (bIsSlideDone == false)
		GetCharacterMovement()->GroundFriction = InitialFriction;
}

void ARocketPlayer::RPCDirectHit_Implementation()
{
	BlueprintConcussion();
}

void ARocketPlayer::ReleaseMovement(FVector DeathLocation)
{ 
	//this->GetCapsuleComponent()->DestroyComponent();
	//this->GetCharacterMovement()->DisableMovement();
	//this->GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	/*for (USkeletalMeshComponent* SkeleMesh : SkeleMeshes)
	{
		this->GetMesh()->SetAnimInstanceClass(nullptr);
		this->GetMesh()->SetAllBodiesSimulatePhysics(true);
	}*/

	//if (APlayerController* PlayerController = Cast<APlayerController>(this->Controller))
	//{
	//	DisableInput(PlayerController);
	//	if (DeathSpecView != nullptr)
	//	{
	//		DeathSpecView->SetActorLocation(DeathLocation);
	//		bShouldDeathSpecRotUpdate = true;

	//		//PlayerController->Possess(DeathSpecView);
	//		PlayerController->SetViewTarget(DeathSpecView);

	//	}
	//}
		
}

void ARocketPlayer::CallDestroy()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		this->Destroy();
	}
}

void ARocketPlayer::Die()
{
	//FTimerHandle DeathTime;
	//GetWorldTimerManager().SetTimer(DeathTime, this, &ARocketPlayer::Respawn, RespawnDelay, false);
	if (GetLocalRole() == ROLE_Authority)
	{
		if (ARocketPlayerController* pRocketController = Cast<ARocketPlayerController>(GetController()))
		{
			pRocketController->StartUpdatingDeathSpec(GetActorLocation());
		}
	
		//RPCDieMulti();

		// Start destroy timer to remove actor from world
		//GetWorld()->GetTimerManager().SetTimer(DestroyHandle, this, &ARocketPlayer::CallDestroy, 10.0f, false);

		// Respawn
		
	
		AGameModeBase* GM = GetWorld()->GetAuthGameMode();

		if (ARocketGameModeBase* GameMode = Cast<ARocketGameModeBase>(GM))
		{
			GameMode->Respawn(GetController());
		}
	}
}

bool ARocketPlayer::RPCDieMulti_Validate()
{
	return true;
}

void ARocketPlayer::RPCDieMulti_Implementation()
{
	//Ragdoll and stuff
	GetCapsuleComponent()->DestroyComponent();
	this->GetCharacterMovement()->DisableMovement();
	this->GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	this->GetMesh()->SetAllBodiesSimulatePhysics(true);
}

void ARocketPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARocketPlayer, PlayerPoints);
}

