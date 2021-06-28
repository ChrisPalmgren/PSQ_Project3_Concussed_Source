// Copyright (c) 2020 Tension Graphics AB

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerRespawnPoint.generated.h"

UCLASS()
class PROJECTROCKET_API APlayerRespawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerRespawnPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

};
