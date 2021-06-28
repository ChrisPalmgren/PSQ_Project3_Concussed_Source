// Copyright (c) 2020 Tension Graphics AB


#include "ControlPoint.h"
#include "Components/SpotLightComponent.h"
#include "EngineUtils.h"
#include "Engine/Engine.h"
#include "Net/Unrealnetwork.h"
#include "GameTimerStateBase.h"
#include "RocketPlayerState.h"
#include "Timermanager.h"

// Sets default values
AControlPoint::AControlPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CylinderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CylinderMesh"));
	RootComponent = CylinderMesh;
}

// Called when the game starts or when spawned
void AControlPoint::BeginPlay()
{
	Super::BeginPlay();
	CylinderMesh->OnComponentBeginOverlap.AddDynamic(this, &AControlPoint::OnBeginOverlap);
	CylinderMesh->OnComponentEndOverlap.AddDynamic(this, &AControlPoint::OnEndOverlap);

	bTeamTwoCapturing = false;
	bTeamOneCapturing = false;
	bTwoHasCaptured = false;
	bOneHasCaptured = false;
	bStartTicking = false;

	MaxCaptureValue = CaptureDelay;

	GetWorldTimerManager().SetTimer(SetScoreHandle, this, &AControlPoint::AddScore, 1.0f, true);
	GetWorldTimerManager().PauseTimer(SetScoreHandle);
}

void AControlPoint::AddScore()
{
	AddingScore = 1;
	AGameStateBase* GS = GetWorld()->GetGameState();
	if (AGameTimerStateBase* GameState = Cast<AGameTimerStateBase>(GS))
	{
		GameState->UpdateScore(bOneHasCaptured, bTwoHasCaptured, AddingScore, SetScoreHandle);
	}
}

void AControlPoint::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ARocketPlayer* Player = Cast<ARocketPlayer>(OtherActor))
	{
		TArray<FHitResult> HitArray;
		FVector Start = this->GetActorLocation();
		FVector End = Start;

		const bool bSweepHit = GetWorld()->SweepMultiByChannel
		(
			HitArray,
			Start,
			End,
			FQuat::Identity,
			ECC_WorldStatic,
			CylinderMesh->GetCollisionShape()
		);

		if (bSweepHit == true)
		{
			for (const FHitResult HitResult : HitArray)
			{
				OtherActor = HitResult.GetActor();
				ARocketPlayer* PlayerActor = Cast<ARocketPlayer>(OtherActor);
				if (PlayerActor != nullptr)
				{
					ARocketPlayerState* PlayerState = Cast<ARocketPlayerState>(PlayerActor->GetPlayerState());
					if (PlayerState != nullptr)
					{
						if (PlayersInCP.Contains(PlayerState) == false)
							PlayersInCP.Add(PlayerState);
					}
				}
			}
		}

		bTeamOneCapturing = false;
		bTeamTwoCapturing = false;

		for (ARocketPlayerState* PlayerCheck : PlayersInCP)
		{
			if (PlayerCheck != nullptr)
			{
				if (PlayerCheck->iTeamID == 1)
					bTeamOneCapturing = true;
				if (PlayerCheck->iTeamID == 2)
					bTeamTwoCapturing = true;
			}
		}
		PlayersInCP.Empty();

		if (bTeamOneCapturing == true && bTeamTwoCapturing == true)
		{
			CapState = DUO_CAP;
		}
		else if (bTeamOneCapturing == true && bTeamTwoCapturing == false)
		{
			CapState = ONE_CAP;
		}
		else if (bTeamTwoCapturing == true && bTeamOneCapturing == false)
		{
			CapState = TWO_CAP;
		}
		if (bTeamTwoCapturing == false && bTeamOneCapturing == false)
		{
			CapState = NONE;
		}

		bStartTicking = true;
	}
}

void AControlPoint::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ARocketPlayer* LeavingPlayer = Cast<ARocketPlayer>(OtherActor))
	{
		ARocketPlayerState* LeavingPlayerState = Cast<ARocketPlayerState>(LeavingPlayer->GetPlayerState());

		TArray<FHitResult> HitArray;
		FVector Start = this->GetActorLocation();
		FVector End = Start;

		const bool bSweepHit = GetWorld()->SweepMultiByChannel
		(
			HitArray,
			Start,
			End,
			FQuat::Identity,
			ECC_WorldStatic,
			CylinderMesh->GetCollisionShape()
		);

		if (bSweepHit == true)
		{
			for (const FHitResult HitResult : HitArray)
			{
				AActor* OtherPlayer = HitResult.GetActor();
				ARocketPlayer* PlayerActor = Cast<ARocketPlayer>(OtherPlayer);
				if (PlayerActor != nullptr)
				{
					ARocketPlayerState* PlayerActorState = Cast<ARocketPlayerState>(PlayerActor->GetPlayerState());
					if (LeavingPlayerState != nullptr && PlayerActorState != nullptr)
					{
						if (RemainingPlayers.Contains(PlayerActorState) == false)
							RemainingPlayers.Add(PlayerActorState);
						if (RemainingPlayers.Contains(LeavingPlayerState) == true)
							RemainingPlayers.Remove(LeavingPlayerState);
					}
				}
			}
		}

		bTeamOneCapturing = false;
		bTeamTwoCapturing = false;

		for (ARocketPlayerState* PlayerCheck : RemainingPlayers)
		{
			if (PlayerCheck != nullptr)
			{
				if (PlayerCheck->iTeamID == 1)
					bTeamOneCapturing = true;
				if (PlayerCheck->iTeamID == 2)
					bTeamTwoCapturing = true;
			}
		}
		RemainingPlayers.Empty();

		if (bTeamOneCapturing == true && bTeamTwoCapturing == true)
		{
			CapState = DUO_CAP;
		}
		if (bTeamOneCapturing == true && bTeamTwoCapturing == false)
		{
			CapState = ONE_CAP;
		}
		else if (bTeamTwoCapturing == true && bTeamOneCapturing == false)
		{
			CapState = TWO_CAP;
		}
		if (bTeamTwoCapturing == false && bTeamOneCapturing == false)
		{
			CapState = NONE;
		}
		
		bStartTicking = true;
	}
}

void AControlPoint::CaptureCounter(float DeltaTime)
{
	if (bStartTicking == true)
	{
		switch (CapState)
		{
		case AControlPoint::DUO_CAP:
			bStartTicking = false;
			break;
		case AControlPoint::ONE_CAP:
			if (bOneHasCaptured == true)
			{
				CapState = NONE;
			}

			dt_temp += DeltaTime;
			CaptureValue = dt_temp;
			if (CaptureValue >= CaptureDelay)
			{
				bOneHasCaptured = true;
				bTwoHasCaptured = false;

				dt_temp = 0.0f;
				CaptureValue = dt_temp;

				if (GetWorldTimerManager().IsTimerActive(SetScoreHandle) == false)
					GetWorldTimerManager().UnPauseTimer(SetScoreHandle);

				bStartTicking = false;
			}
			break;
		case AControlPoint::TWO_CAP:
			if (bTwoHasCaptured == true)
			{
				CapState = NONE;
			}

			dt_temp += DeltaTime;
			CaptureValue = dt_temp;
			if (CaptureValue >= CaptureDelay)
			{
				bOneHasCaptured = false;
				bTwoHasCaptured = true;

				dt_temp = 0.0f;
				CaptureValue = dt_temp;

				if (GetWorldTimerManager().IsTimerActive(SetScoreHandle) == false)
					GetWorldTimerManager().UnPauseTimer(SetScoreHandle);

				bStartTicking = false;
			}
			break;
		case AControlPoint::NONE:
			if (CaptureValue > 0.0f)
			{
				dt_temp -= (DeltaTime * CaptureDecreaseMultiplier);
				CaptureValue = dt_temp;
				if (CaptureValue <= 0.0f)
				{
					CaptureValue = 0.0f;
					bStartTicking = false;
				}
			}
			break;
		}
	}
}

// Called every frame
void AControlPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CaptureCounter(DeltaTime);
}

