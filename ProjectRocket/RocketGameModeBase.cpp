// Copyright (c) 2020 Tension Graphics AB


#include "RocketGameModeBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Camera/CameraActor.h"
#include "ControlPoint.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "GameTimerStateBase.h"
#include "RocketPlayerState.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"

void ARocketGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerRespawnPoint::StaticClass(), PlayerRespawnActors);

	FName Team1_Tag = FName(TEXT("Team1"));
	FName Team2_Tag = FName(TEXT("Team2"));

	for (AActor* PlayerRespawn : PlayerRespawnActors)
	{
		if (APlayerRespawnPoint* pRP = Cast<APlayerRespawnPoint>(PlayerRespawn))
		{
			if (pRP->GetRootComponent()->ComponentHasTag(Team1_Tag))
			{
				Team1_RespawnList.Add(pRP);
			}
			else if (pRP->GetRootComponent()->ComponentHasTag(Team2_Tag))
			{
				Team2_RespawnList.Add(pRP);
			}
		}
	}

	GetWorldTimerManager().SetTimer(PendingStartHandle, this, &ARocketGameModeBase::PendingStartRocketGame, 1.0f, true);

	m_pGameState = GetGameState<AGameTimerStateBase>();
}

void ARocketGameModeBase::PendingStartRocketGame()
{
	if (PendingStartTime == 1)
	{
		bGamehasStarted = true;
	}
	else if (PendingStartTime == 0)
	{
		GetWorldTimerManager().SetTimer(GameTimerHandle, this, &ARocketGameModeBase::UpdateTimer, 1.0f, true);
		if (GetWorldTimerManager().IsTimerActive(PendingStartHandle))
		{
			GetWorldTimerManager().ClearTimer(PendingStartHandle);
			return;
		}
		return;
	}
	--PendingStartTime;

	if (m_pGameState != nullptr)
	{
		m_pGameState->UpdatePendingStart(PendingStartTime);
	}
}

void ARocketGameModeBase::UpdateTimer()
{
	if (m_pGameState != nullptr)
	{
		if (SecOnes == 9)
		{
			if (SecTens == 5)
			{
				if (MinOnes == 9)
				{
					if (MinTens == 5)
					{
						MinTens = 0;
						MinOnes = 0;
						SecTens = 0;
						SecOnes = 0;
						m_pGameState->UpdateTime(SecOnes, SecTens, MinOnes, MinTens);
						return;
					}
					SecOnes = 0;
					SecTens = 0;
					MinOnes = 0;
					++MinTens;
					m_pGameState->UpdateTime(SecOnes, SecTens, MinOnes, MinTens);
					return;
				}
				SecOnes = 0;
				SecTens = 0;
				++MinOnes;
				m_pGameState->UpdateTime(SecOnes, SecTens, MinOnes, MinTens);
				return;
			}
			SecOnes = 0;
			++SecTens;
			m_pGameState->UpdateTime(SecOnes, SecTens, MinOnes, MinTens);
			return;
		}
		++SecOnes;
		m_pGameState->UpdateTime(SecOnes, SecTens, MinOnes, MinTens);
		return;
	}
		
}

void ARocketGameModeBase::GameOver()
{
	bGameOver = true;

	if (m_pGameState != nullptr)
		m_pGameState->ComparePoints();

	GetWorldTimerManager().SetTimer(EndGameTimer, this, &ARocketGameModeBase::TravelToLobby, 4.0f, false);
}

void ARocketGameModeBase::TravelToLobby()
{
	GetWorld()->ServerTravel(TEXT("LobbyMap"));
}

void ARocketGameModeBase::SpawnStart(AController* pPlayerController)
{
	if (pPlayerController)
	{
		//int iRandomNumber = FMath::RandRange(0, PlayerRespawnActors.Num() - 1);    

		/*FVector SpawnLocation = PlayerRespawnActors[iRandomNumber]->GetActorLocation();
		FRotator SpawnRotation = PlayerRespawnActors[iRandomNumber]->GetActorRotation();

		FTransform SpawnTransform;
		SpawnTransform.SetRotation(SpawnRotation.Quaternion());
		SpawnTransform.SetLocation(SpawnLocation);*/

		if (ARocketPlayerController* pRocketPlayerController = Cast<ARocketPlayerController>(pPlayerController))
		{
			if (ARocketPlayerState* pRocketPlayerState = Cast<ARocketPlayerState>(pRocketPlayerController->PlayerState))
			{
				APlayerRespawnPoint* RespawnPoint = GetSpawnPoint(pRocketPlayerState);
				if (RespawnPoint != nullptr)
				{
					FVector SpawnLocation = RespawnPoint->GetActorLocation();
					FRotator SpawnRotation = RespawnPoint->GetActorRotation();

					FTransform SpawnTransform;

					SpawnTransform.SetRotation(SpawnRotation.Quaternion());
					SpawnTransform.SetLocation(SpawnLocation);

					if (ARocketPlayer* pNewPawn = GetWorld()->SpawnActorDeferred<ARocketPlayer>(PlayerActorClass, SpawnTransform))
					{
						pRocketPlayerController->RPCSetControlRotation(SpawnRotation);
						pPlayerController->Possess(pNewPawn);
						UGameplayStatics::FinishSpawningActor(pNewPawn, SpawnTransform);
						//pNewPawn->FinishSpawning(SpawnTransform);
					}
					else
					{
						GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Blue, TEXT("Spawn actor deferred failed"));
					}
				}
				else 
				{
					GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Blue, TEXT("NO SPAWNPOINT"));
				}
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Blue, TEXT("Playerstate cast failed"));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Blue, TEXT("Player controller cast failed"));
		}
	}
}

void ARocketGameModeBase::Respawn(AController* pController)
{
	if (bGameOver)
		return;

	if (GetLocalRole() == ROLE_Authority)
	{
		if (pController != nullptr)
		{
			if (ARocketPlayerController* RocketPlayerController = Cast<ARocketPlayerController>(pController))
			{
				FTimerDelegate RespawnDelegate;
				RespawnDelegate.BindUFunction(this, FName("Spawn"), pController);
				GetWorld()->GetTimerManager().SetTimer(RocketPlayerController->RespawnHandle, RespawnDelegate, RespawnTime, false);
			}
		}
	}
}

void ARocketGameModeBase::Spawn(AController* pController)
{
	if (bGameOver)
		return;

	if (ARocketPlayerController* pRocketPlayerController = Cast<ARocketPlayerController>(pController))
	{
		pRocketPlayerController->StopUpdatingDeathSpec();
		if (ARocketPlayerState* pRocketPlayerState = Cast<ARocketPlayerState>(pRocketPlayerController->PlayerState))
		{
			APlayerRespawnPoint* RespawnPoint = GetSpawnPoint(pRocketPlayerState);
			if (RespawnPoint != nullptr)
			{
				FVector SpawnLocation = RespawnPoint->GetActorLocation();
				FRotator SpawnRotation = RespawnPoint->GetActorRotation();

				//FTransform SpawnTransform;

				//SpawnTransform.SetRotation(SpawnRotation.Quaternion());
				//SpawnTransform.SetLocation(SpawnLocation);

				//if (ARocketPlayer* pNewPawn = GetWorld()->SpawnActorDeferred<ARocketPlayer>(PlayerActorClass, SpawnTransform))
				//{
					
					if (ARocketPlayer* pPlayerPawn = Cast<ARocketPlayer>(pRocketPlayerController->GetPawn()))
					{
						pPlayerPawn->SetActorLocation(SpawnLocation);
						pPlayerPawn->BlueprintSpawn();
					}
					
					pRocketPlayerController->RPCSetControlRotation(SpawnRotation);
					//pRocketPlayerController->Possess(pNewPawn);
					//UGameplayStatics::FinishSpawningActor(pNewPawn, SpawnTransform);
					//pNewPawn->FinishSpawning(SpawnTransform);
				//}
			}
		}
	}
}

void ARocketGameModeBase::Regen(APawn* Pawn)
{
	/*
	if (ARocketPlayer* RegenPlayer = GetWorld()->SpawnActorDeferred<ARocketPlayer>(PlayerActorClass, SpawnTransform))
	{
		//RegenPlayer->SetActorLocation(RespawnLocation);
		ARocketPlayerController* FetchedPlayerController = Cast<ARocketPlayerController>(Pawn->GetController());
		if (FetchedPlayerController != nullptr)
		{
			//FetchedPlayerController->bShouldDeathSpecRotUpdate = false;
			Pawn->DetachFromControllerPendingDestroy();
			FetchedPlayerController->SetViewTarget(RegenPlayer);
			FetchedPlayerController->Possess(RegenPlayer);
			//EnableInput(FetchedPlayerController);
			FetchedPlayerController->SetControlRotation(SpawnTransform.GetRotation().Rotator());
			UGameplayStatics::FinishSpawningActor(RegenPlayer, SpawnTransform);
			//RegenPlayer->FinishSpawning(SpawnTransform);
			
			//if (ARocketPlayer* DeadPlayer = Cast<ARocketPlayer>(Pawn))
			//{
				//FTimerHandle DestroyHandle;
				//GetWorldTimerManager().SetTimer(DestroyHandle, DeadPlayer, &ARocketPlayer::CallDestroy, false);
			//}
		}
	}
	*/
}


// Assign team on post login
void ARocketGameModeBase::PostLogin(APlayerController *NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ARocketPlayerState* NewPlayerState = NewPlayer->GetPlayerState<ARocketPlayerState>();

	if (GetTeamSize(1) < GetTeamSize(2))
	{
		NewPlayerState->iTeamID = 1;
	}
	else
	{
		NewPlayerState->iTeamID = 2;
	}

}

int ARocketGameModeBase::GetTeamSize(int iTeamID)
{
	int iTeamCount = 0;

	if (m_pGameState != nullptr)
	{
		TArray<APlayerState*> pConnectedPlayers = m_pGameState->PlayerArray;

		if (pConnectedPlayers.Num() > 1)
		{
			for (int iPlayers = 0; iPlayers < pConnectedPlayers.Num(); ++iPlayers)
			{
				ARocketPlayerState* pPlayer = Cast<ARocketPlayerState>(pConnectedPlayers[iPlayers]); 

				if (pPlayer != nullptr)
				{
					if (pPlayer->iTeamID == iTeamID)
					{
						++iTeamCount;
					}
				}
			}
		}
	}

	return iTeamCount;
}

APlayerRespawnPoint* ARocketGameModeBase::GetSpawnPoint(class ARocketPlayerState* PlayerState)
{
	FName Team1_Tag = FName(TEXT("Team1"));
	FName Team2_Tag = FName(TEXT("Team2"));

	if (PlayerState->iTeamID == 1)
	{
		int Slot = FMath::RandRange(0, Team1_RespawnList.Num() - 1);
		APlayerRespawnPoint* pSelectedSpawnPoint = Cast<APlayerRespawnPoint>(Team1_RespawnList[Slot]);
		return pSelectedSpawnPoint;
	}
	else
	{
		int Slot = FMath::RandRange(0, Team2_RespawnList.Num() - 1);
		APlayerRespawnPoint* pSelectedSpawnPoint = Cast<APlayerRespawnPoint>(Team2_RespawnList[Slot]);
		return pSelectedSpawnPoint;
	}
}
