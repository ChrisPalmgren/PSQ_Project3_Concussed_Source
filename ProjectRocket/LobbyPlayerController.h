// Copyright (c) 2020 Tension Graphics AB

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTROCKET_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	AActor* pLobbySpawnPoint;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void RPCSwitchTeam();
	void RPCSwitchTeam_Implementation();

	UFUNCTION(Server, Reliable)
	void RPCSetControlRotation(FRotator controllerRotation);
	void RPCSetControlRotation_Implementation(FRotator controllerRotation);

private:	
	UFUNCTION(Server, Reliable)
	void RPCSpawnOnStart();
	void RPCSpawnOnStart_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void RPCSetControlRotationMulti(FRotator controllerRotation);
	void RPCSetControlRotationMulti_Implementation(FRotator controllerRotation);
};
