// Copyright (c) 2020 Tension Graphics AB

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DeathPawn.generated.h"

UCLASS()
class PROJECTROCKET_API ADeathPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADeathPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
