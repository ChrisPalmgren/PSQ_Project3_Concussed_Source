// Copyright (c) 2020 Tension Graphics AB


#include "KillZone.h"
#include "Camera/CameraActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "RocketPlayerController.h"
#include "TimerManager.h"

// Sets default values
AKillZone::AKillZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	RootComponent = BoxCollision;
}

// Called when the game starts or when spawned
void AKillZone::BeginPlay()
{
	Super::BeginPlay();

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AKillZone::OnBeginOverlap);
}

void AKillZone::OnBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ARocketPlayer* Player = Cast<ARocketPlayer>(OtherActor))
	{
		if (GetLocalRole() == ROLE_Authority)
		{
			Player->Die();
		}
	}
}


