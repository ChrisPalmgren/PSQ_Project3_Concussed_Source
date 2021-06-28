// Copyright (c) 2020 Tension Graphics AB

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RocketPlayer.h"
#include "ControlPoint.generated.h"

UCLASS()
class PROJECTROCKET_API AControlPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AControlPoint();

	UFUNCTION()
	void OnBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	int AddingScore;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
		bool bTwoHasCaptured;
	UPROPERTY(BlueprintReadOnly)
		bool bOneHasCaptured;
	UPROPERTY(BlueprintReadOnly)
		float CaptureValue = 0.0f;
	UPROPERTY(EditAnywhere)
		float CaptureDecreaseMultiplier = 0.2f;
	UPROPERTY(BlueprintReadOnly)
		float MaxCaptureValue = 0.0f;
	UPROPERTY(BlueprintReadOnly)
		bool bTeamTwoCapturing;
	UPROPERTY(BlueprintReadOnly)
		bool bTeamOneCapturing;

private:
	enum CaptureStates { DUO_CAP, ONE_CAP, TWO_CAP, NONE };
	CaptureStates CapState;
	void CaptureCounter(float DeltaTime);

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* CylinderMesh;

	TArray<class ARocketPlayerState*> PlayersInCP;
	TArray<class ARocketPlayerState*> RemainingPlayers;

	bool bStartTicking;
	float dt_temp = 0.0f;
	UPROPERTY(EditAnywhere)
		float CaptureDelay = 5.0f;
	
	FTimerHandle SetScoreHandle;
	FTimerHandle BlueScoreHandle;

	void AddScore();

	FTimerHandle CaptureTimeHandle;
};
