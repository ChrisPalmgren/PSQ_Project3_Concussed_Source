// Copyright (c) 2020 Tension Graphics AB


#include "RocketPlayerState.h"

#include "Net/Unrealnetwork.h"

void ARocketPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARocketPlayerState, iTeamID);
}

void ARocketPlayerState::CopyProperties(class APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	if (IsValid(PlayerState))
	{
		ARocketPlayerState* NewPlayerState = Cast<ARocketPlayerState>(PlayerState);        
		if (NewPlayerState)
		{            
			NewPlayerState->iTeamID = this->iTeamID;            
		}
	}
}