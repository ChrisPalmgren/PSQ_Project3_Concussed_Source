// Copyright (c) 2020 Tension Graphics AB

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "RocketPlayerController.h"
#include "RocketPlayer.h"
#include "GameTimerStateBase.generated.h"

UCLASS()
class PROJECTROCKET_API AGameTimerStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	AGameTimerStateBase();

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;

	void UpdatePendingStart(int Time);
	void UpdateTime(int SecOnes, int SecTens, int MinOnes, int MinTens);
	void UpdateScore(bool One, bool Two, int Score, FTimerHandle ScoreHandle);

	//void UpdateCaptureValueNormal(float CapValue, float MaxCapValue);
	//UFUNCTION(Server, Reliable)
	//void RPCUpdateCaptureValueNormal(float CaptureValNorm);
	//void RPCUpdateCaptureValueNormal_Implementation(float CaptureValNorm);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = GameRules)
	int PendingStartTime = 10;

	UPROPERTY(BlueprintReadOnly, Replicated)
	int GameSecOnes = 0;
	UPROPERTY(BlueprintReadOnly, Replicated)
	int GameSecTens = 0;
	UPROPERTY(BlueprintReadOnly, Replicated)
	int GameMinOnes = 0;
	UPROPERTY(BlueprintReadOnly, Replicated)
	int GameMinTens = 0;
	UPROPERTY(EditAnywhere, Category = GameRules)
	int MaxScore = 500;

	UFUNCTION(Server, Reliable)
	void RPCPendingStart(int Time);
	void RPCPendingStart_Implementation(int Time);

	UFUNCTION(Server, Reliable)
	void RPCCounter(int SecOnes, int SecTens, int MinOnes, int MinTens);
	void RPCCounter_Implementation(int SecOnes, int SecTens, int MinOnes, int MinTens);

	void ComparePoints();

	UPROPERTY(EditAnywhere, Category = PlayerUI)
	TSubclassOf<class UUserWidget> WinScreenClass;
	UUserWidget* WinScreen;

	UPROPERTY(EditAnywhere, Category = PlayerUI)
	TSubclassOf<class UUserWidget> LoseScreenClass;
	UUserWidget* LoseScreen;

	UFUNCTION(NetMultiCast, Reliable, WithValidation)
	void MulticastGameOverUI();
	bool MulticastGameOverUI_Validate();
	void MulticastGameOverUI_Implementation();

	UPROPERTY(BlueprintReadOnly, Replicated, Category = Score)
	int TeamOneScore = 0;
	UPROPERTY(BlueprintReadOnly, Replicated, Category = Score)
	int TeamTwoScore = 0;

	UFUNCTION(Server, Reliable)
	void RPCSetScore(bool One, bool Two, int Score, FTimerHandle ScoreHandle);
	void RPCSetScore_Implementation(bool One, bool Two, int Score, FTimerHandle ScoreHandle);

private:
	TArray<AActor*> RocketPlayers;

protected:
	UPROPERTY(BlueprintReadOnly)
		TArray<AActor*> Controlpoints;
	UPROPERTY(BlueprintReadWrite, Replicated)
		float CaptureValNormal = 0.0f;
};
