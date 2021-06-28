// Copyright (c) 2020 Tension Graphics AB


#include "LobbyPlayerController.h"
#include "LobbyGameModeBase.h"

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController() == true)
	{
		RPCSpawnOnStart();
	}
}

void ALobbyPlayerController::RPCSpawnOnStart_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		AGameModeBase* GM = GetWorld()->GetAuthGameMode();

		if (ALobbyGameModeBase * GameMode = Cast<ALobbyGameModeBase>(GM))
		{
			if (GetPlayerState<ARocketPlayerState>())
			{
				GameMode->SpawnStart(this);
			}	
		}
	}
}

void ALobbyPlayerController::RPCSwitchTeam_Implementation() 
{
	if (ARocketPlayerState * pPlayerState = GetPlayerState<ARocketPlayerState>())
	{
		if (pPlayerState->iTeamID == 1)
		{
			pPlayerState->iTeamID = 2;
		}
		else
		{
			pPlayerState->iTeamID = 1;
		}
	}
}

void ALobbyPlayerController::RPCSetControlRotation_Implementation(FRotator controllerRotation)
{
	RPCSetControlRotationMulti(controllerRotation);
}

void ALobbyPlayerController::RPCSetControlRotationMulti_Implementation(FRotator controllerRotation)
{
	if (IsLocalController())
	{
		ControlRotation = controllerRotation;
	}
}
