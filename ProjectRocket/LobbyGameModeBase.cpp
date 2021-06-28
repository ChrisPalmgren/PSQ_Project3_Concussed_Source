// Copyright (c) 2020 Tension Graphics AB


#include "LobbyGameModeBase.h"
#include "LobbyPlayerController.h"
//#include "Net/Unrealnetwork.h"
#include "Kismet/GameplayStatics.h"

void ALobbyGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	m_pGameState = GetGameState<ALobbyGameStateBase>();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerRespawnPoint::StaticClass(), LobbySpawnPoints);



	/*
	if (m_pGameState != nullptr)
	{
		TArray<APlayerState*> pConnectedPlayers = m_pGameState->PlayerArray;

		if (pConnectedPlayers.Num() == 1)
		{
			ARocketPlayerState* pPlayer = Cast<ARocketPlayerState>(pConnectedPlayers[0]); 
			if (pPlayer != nullptr)
			{
				pPlayer->iTeamID = 1;
			}
		}
	*/
}

void ALobbyGameModeBase::PostLogin(APlayerController* pNewPlayer)
{
	Super::PostLogin(pNewPlayer);

	ARocketPlayerState* pNewPlayerState = pNewPlayer->GetPlayerState<ARocketPlayerState>();

	//NewPlayerState->iTeamID = m_pGameState->PlayerArray.Num();
	
	if (GetTeamSize(1) < GetTeamSize(2))
	{
		pNewPlayerState->iTeamID = 1;
	}
	else
	{
		pNewPlayerState->iTeamID = 2;
	}
	
}

int ALobbyGameModeBase::GetTeamSize(int iTeamID)
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

void ALobbyGameModeBase::SpawnStart(AController* pPlayerController)
{
	if (pPlayerController)
	{
		if (LobbySpawnPoints.Num() > 0)
		{
			if (ALobbyPlayerController * pLobbyController = Cast<ALobbyPlayerController>(pPlayerController))
			{
				FVector SpawnLocation = LobbySpawnPoints[0]->GetActorLocation();
				FRotator SpawnRotation = LobbySpawnPoints[0]->GetActorRotation();

				pLobbyController->pLobbySpawnPoint = LobbySpawnPoints[0];
				LobbySpawnPoints.RemoveAt(0, 1, true);

				FTransform SpawnTransform;
				SpawnTransform.SetRotation(SpawnRotation.Quaternion());
				SpawnTransform.SetLocation(SpawnLocation);

				if (ARocketPlayer * pNewPawn = GetWorld()->SpawnActorDeferred<ARocketPlayer>(PlayerActorClass, SpawnTransform))
				{
					pLobbyController->RPCSetControlRotation(SpawnRotation);
					pLobbyController->Possess(pNewPawn);
					pNewPawn->FinishSpawning(SpawnTransform);
				}
			}
		}
	}
}

void ALobbyGameModeBase::Logout(AController* pPlayerController) 
{
	if (ALobbyPlayerController * pLobbyController = Cast<ALobbyPlayerController>(pPlayerController))
	{
		LobbySpawnPoints.Add(pLobbyController->pLobbySpawnPoint);
	}
}
