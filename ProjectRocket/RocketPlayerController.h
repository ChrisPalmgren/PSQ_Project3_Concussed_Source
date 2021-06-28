// Copyright (c) 2020 Tension Graphics AB

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RocketPlayer.h"
#include "RocketPlayerController.generated.h"

/**
 *
 */
UCLASS()
class PROJECTROCKET_API ARocketPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ARocketPlayerController();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaTime) override;

	void CreateWinOrLoseScreen();
	void ToggleInGameMenu();

	bool bIsWinner = false;
	bool bGameOver = false;
	bool bShouldDeathSpecRotUpdate = false;

	UFUNCTION(Client, Reliable)
	void StartUpdatingDeathSpec(FVector DeathLocation);
	void StartUpdatingDeathSpec_Implementation(FVector DeathLocation);
	UFUNCTION(Client, Reliable)
	void StopUpdatingDeathSpec();
	void StopUpdatingDeathSpec_Implementation();


	// Called to bind functionality to input
	virtual void SetupInputComponent() override;

	UFUNCTION(Server, Reliable)
	void RPCSetControlRotation(FRotator controllerRotation);
	void RPCSetControlRotation_Implementation(FRotator controllerRotation);

	UFUNCTION(Server, Reliable)
	void RPCActivateMovement();
	void RPCActivateMovement_Implementation();

	FTimerHandle RespawnHandle;

	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintRespawnTimer();
	void BlueprintRespawnTimer_Implementation();

	UPROPERTY(BlueprintReadOnly)
	UUserWidget* PlayerUI;
private:
	void CreatePlayerUI();
	void DestroyPlayerUI();

	UPROPERTY(EditAnywhere, Category = PlayerUI)
	TSubclassOf<class UUserWidget> PlayerUIClass;

	UPROPERTY(EditAnywhere, Category = PlayerUI)
	TSubclassOf<class UUserWidget> WinScreenClass;
	UPROPERTY()
	UUserWidget* WinScreen;
	UPROPERTY(EditAnywhere, Category = PlayerUI)
	TSubclassOf<class UUserWidget> LoseScreenClass;
	UPROPERTY()
	UUserWidget* LoseScreen;
	UPROPERTY(EditAnywhere, Category = PlayerUI)
	TSubclassOf<class UUserWidget> InGameMenuClass;
	UPROPERTY()
	UUserWidget* InGameMenu;

	FTimerHandle RestartHandle;
	UPROPERTY(EditAnywhere, Category = "Restart")
	float RestartDelay = 3.0f;

	UFUNCTION(Server, Reliable)
	void RPCSpawnOnStart();
	void RPCSpawnOnStart_Implementation();

	class ACameraActor* DeathSpecView;

	UFUNCTION(NetMulticast, Reliable)
	void RPCSetControlRotationMulti(FRotator controllerRotation);
	void RPCSetControlRotationMulti_Implementation(FRotator controllerRotation);

	UFUNCTION(NetMulticast, Reliable)
	void RPCActivateMovementMulti();
	void RPCActivateMovementMulti_Implementation();
};
