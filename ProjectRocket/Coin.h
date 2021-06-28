// Copyright (c) 2020 Tension Graphics AB

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerState.h"
#include "Coin.generated.h"


UCLASS()
class PROJECTROCKET_API ACoin : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACoin();

	UFUNCTION()
	void OnBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BoxCollision", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SphereCollision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StaticMesh", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMesh;
};
