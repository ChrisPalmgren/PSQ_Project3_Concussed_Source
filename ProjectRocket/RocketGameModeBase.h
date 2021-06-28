// Copyright (c) 2020 Tension Graphics AB

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Coin.h"
#include "CoinSpawnPoint.h"
#include "PlayerRespawnPoint.h"
#include "RocketPlayer.h"
#include "RocketPlayerController.h"
#include "GameTimerStateBase.h"
#include "RocketGameModeBase.generated.h"

UCLASS()
class PROJECTROCKET_API ARocketGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

private:
	TArray<AActor*> PlayerRespawnActors;
	TArray<AActor*> Team1_RespawnList;
	TArray<AActor*> Team2_RespawnList;


	FTimerHandle PendingStartHandle;
	FTimerHandle GameTimerHandle;

	FTimerHandle EndGameTimer;

	AGameTimerStateBase* m_pGameState;

	void PostLogin(APlayerController* NewPlayer) override;
	int GetTeamSize(int iTeamID);
	APlayerRespawnPoint* GetSpawnPoint(class ARocketPlayerState* PlayerState);

	void PendingStartRocketGame();
public:
	virtual void BeginPlay() override;

	void UpdateTimer();

	void GameOver();

	void TravelToLobby();

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> CoinActor;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> PlayerActorClass;
	UPROPERTY(EditAnywhere)
		TSubclassOf<APlayerRespawnPoint> Team1_Respawn_Class;
	APlayerRespawnPoint* Team1_Respawn;
	UPROPERTY(EditAnywhere)
		TSubclassOf<APlayerRespawnPoint> Team2_Respawn_Class;
	APlayerRespawnPoint* Team2_Respawn;

	void Respawn(AController* pController);
	void SpawnStart(AController* pPlayerController);
	void Regen(APawn* Pawn);

	UFUNCTION()
	void Spawn(AController* pController);

	int PendingStartTime;

	int SecOnes = 0;
	int SecTens = 0;
	int MinOnes = 0;
	int MinTens = 0;

	int RespawnTime = 5.0f;
	int BodyDespawnTime = 10.0f;

	bool bGamehasStarted = false;
	bool bGameOver = false;
};
