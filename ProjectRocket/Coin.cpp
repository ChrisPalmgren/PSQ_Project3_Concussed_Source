// Copyright (c) 2020 Tension Graphics AB


#include "Coin.h"
#include "Components/SphereComponent.h"
#include "RocketPlayer.h"
#include "RocketGameModeBase.h"

// Sets default values
ACoin::ACoin()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = false;
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Box Collision"));
	RootComponent = SphereCollision;
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	StaticMesh->SetupAttachment(SphereCollision);
}

// Called when the game starts or when spawned
void ACoin::BeginPlay()
{
	Super::BeginPlay();	
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ACoin::OnBeginOverlap);
	SetReplicates(true);
}

void ACoin::OnBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		ARocketPlayer* Player = Cast<ARocketPlayer>(OtherActor);

		if (Player != nullptr)
		{
			++Player->PlayerPoints;
			UE_LOG(LogTemp, Warning, TEXT("Player: %s, Points: %i"), *Player->GetName(), Player->PlayerPoints);

			AGameModeBase* GM = GetWorld()->GetAuthGameMode();

			if (ARocketGameModeBase * GameMode = Cast<ARocketGameModeBase>(GM))
			{
			}				
		}

		this->Destroy();
	}
}


