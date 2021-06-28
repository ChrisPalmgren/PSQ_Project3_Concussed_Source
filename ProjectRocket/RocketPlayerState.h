// Copyright (c) 2020 Tension Graphics AB

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "RocketPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTROCKET_API ARocketPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	UPROPERTY(Replicated, BlueprintReadOnly)
	int iTeamID = 0;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;

	void CopyProperties(class APlayerState* PlayerState) override;
};