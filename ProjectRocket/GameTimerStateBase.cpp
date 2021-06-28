// Copyright (c) 2020 Tension Graphics AB


#include "GameTimerStateBase.h"

#include "ControlPoint.h"
#include "Engine.h"
#include "Net/Unrealnetwork.h"
#include "RocketGameModeBase.h"
#include "RocketPlayerState.h"
#include "Engine/Engine.h"

AGameTimerStateBase::AGameTimerStateBase()
{
	SetReplicates(true);
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AGameTimerStateBase::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AControlPoint::StaticClass(), Controlpoints);

	AGameModeBase* GM = GetWorld()->GetAuthGameMode();
	if (ARocketGameModeBase* GameMode = Cast<ARocketGameModeBase>(GM))
	{
		GameMode->PendingStartTime = PendingStartTime;
	}
}

void AGameTimerStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGameTimerStateBase, PendingStartTime);

	DOREPLIFETIME(AGameTimerStateBase, GameSecOnes);
	DOREPLIFETIME(AGameTimerStateBase, GameSecTens);
	DOREPLIFETIME(AGameTimerStateBase, GameMinOnes);
	DOREPLIFETIME(AGameTimerStateBase, GameMinTens);

	DOREPLIFETIME(AGameTimerStateBase, TeamOneScore);
	DOREPLIFETIME(AGameTimerStateBase, TeamTwoScore);

	DOREPLIFETIME(AGameTimerStateBase, CaptureValNormal);
}

void AGameTimerStateBase::UpdatePendingStart(int Time)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		RPCPendingStart(Time);
	}
}

void AGameTimerStateBase::UpdateTime(int SecOnes, int SecTens, int MinOnes, int MinTens)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		RPCCounter(SecOnes, SecTens, MinOnes, MinTens);
	}
}

//void AGameTimerStateBase::UpdateCaptureValueNormal(float CapValue, float MaxCapValue)
//{
//	float StoredValue = CapValue / MaxCapValue;
//	RPCUpdateCaptureValueNormal(StoredValue);
//}
//
//void AGameTimerStateBase::RPCUpdateCaptureValueNormal_Implementation(float CaptureValNorm)
//{
//	CaptureValNormal = CaptureValNorm;
//}

void AGameTimerStateBase::RPCPendingStart_Implementation(int Time)
{
	PendingStartTime = Time;
}

void AGameTimerStateBase::RPCCounter_Implementation(int SecOnes, int SecTens, int MinOnes, int MinTens)
{
	//MaxGameTime = Time;

	GameSecOnes = SecOnes;
	GameSecTens = SecTens;
	GameMinOnes = MinOnes;
	GameMinTens = MinTens;

	//if (MaxGameTime < 0.0f) return;
	//else --MaxGameTime;
}

bool AGameTimerStateBase::MulticastGameOverUI_Validate()
{
	return true;
}

void AGameTimerStateBase::MulticastGameOverUI_Implementation()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARocketPlayer::StaticClass(), RocketPlayers);

	ARocketPlayer* Winner = nullptr;

	for (int iPlayer = 0; iPlayer < RocketPlayers.Num(); ++iPlayer)
	{
		ARocketPlayer* PlayerToFetch = Cast<ARocketPlayer>(RocketPlayers[iPlayer]);
		if (PlayerToFetch != nullptr)
		{
			if (ARocketPlayerState* PlayerState = Cast<ARocketPlayerState>(PlayerToFetch->GetPlayerState()))
			{
				if (PlayerState->iTeamID == 1)
				{
					if (TeamOneScore > TeamTwoScore)
					{
						Winner = PlayerToFetch;
						ARocketPlayerController* WinnerController = Cast<ARocketPlayerController>(Winner->GetController());
						if (WinnerController != NULL)
							WinnerController->bIsWinner = true;
					}
				}
				else if (PlayerState->iTeamID == 2)
				{
					if (TeamTwoScore > TeamOneScore)
					{
						Winner = PlayerToFetch;
						ARocketPlayerController* WinnerController = Cast<ARocketPlayerController>(Winner->GetController());
						if (WinnerController != NULL)
							WinnerController->bIsWinner = true;
					}
				}
			}
		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("Number of players: %i"), RocketPlayers.Num());

	
	/*ARocketPlayerController* LoserController = Cast<ARocketPlayerController>(Loser->GetController());
	if (LoserController != NULL)
		LoserController->bIsWinner = false;*/

	ARocketPlayerController* LocalController = Cast<ARocketPlayerController>(GetWorld()->GetFirstPlayerController());

	if (LocalController != NULL)
		LocalController->CreateWinOrLoseScreen();
}

void AGameTimerStateBase::UpdateScore(bool One, bool Two, int Score, FTimerHandle ScoreHandle)
{
	RPCSetScore(One, Two, Score, ScoreHandle);
}

void AGameTimerStateBase::RPCSetScore_Implementation(bool One, bool Two, int Score, FTimerHandle ScoreHandle)
{
	if (One == true)
	{
		TeamOneScore += Score;
	}
	else if (Two == true)
	{
		TeamTwoScore += Score;
	}

	if (TeamOneScore == MaxScore || TeamTwoScore == MaxScore)
	{
		if (GetWorldTimerManager().IsTimerActive(ScoreHandle) == true)
			GetWorldTimerManager().ClearTimer(ScoreHandle);

		if (AGameModeBase* GM = GetWorld()->GetAuthGameMode())
		{
			if (ARocketGameModeBase* GameMode = Cast<ARocketGameModeBase>(GM))
			{
				GameMode->GameOver();
			}
		}
	}
}

void AGameTimerStateBase::ComparePoints()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		MulticastGameOverUI();
	}
}
