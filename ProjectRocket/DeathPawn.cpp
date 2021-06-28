// Copyright (c) 2020 Tension Graphics AB


#include "DeathPawn.h"

// Sets default values
ADeathPawn::ADeathPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADeathPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADeathPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADeathPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

