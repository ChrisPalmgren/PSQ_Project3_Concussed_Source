// Copyright (c) 2020 Tension Graphics AB

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RocketPlayerState.h"
#include "LobbyGameStateBase.h"
#include "PlayerRespawnPoint.h"
#include "RocketPlayer.h"
#include "LobbyGameModeBase.generated.h"


/**
 * 
 */
UCLASS()
class PROJECTROCKET_API ALobbyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	void SpawnStart(AController* pPlayerController);
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> PlayerActorClass;
private:
	ALobbyGameStateBase* m_pGameState;

	void PostLogin(APlayerController* pNewPlayer) override;
	void Logout(AController* pPlayerController) override;
	int GetTeamSize(int iTeamID);

	TArray<AActor*> LobbySpawnPoints;
};
