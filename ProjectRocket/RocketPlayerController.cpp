// Copyright (c) 2020 Tension Graphics AB


#include "RocketPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "RocketGameModeBase.h"
#include "RocketPlayerState.h"
#include "Engine/Engine.h"

ARocketPlayerController::ARocketPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
}

void ARocketPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController() == true)
	{
		CreatePlayerUI();

		DeathSpecView = GetWorld()->SpawnActor<ACameraActor>();

		RPCSpawnOnStart();
	}
	//FTimerHandle Test;
	//GetWorldTimerManager().SetTimer(Test, this, &ARocketPlayerController::PossessRocketPlayer, 1.0f, false);
}

void ARocketPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsLocalController())
	{
		if (bGameOver == false)
		{
			if (bShouldDeathSpecRotUpdate == true && DeathSpecView != nullptr)
			{
				if (APawn* pPlayerPawn = GetPawn())
				{
					FRotator LookAtPlayer = UKismetMathLibrary::FindLookAtRotation(DeathSpecView->GetActorLocation(), pPlayerPawn->GetActorLocation());
					DeathSpecView->SetActorRotation(LookAtPlayer);
				}
			}
		}
	}
}

void ARocketPlayerController::RPCSpawnOnStart_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		AGameModeBase* GM = GetWorld()->GetAuthGameMode();

		if (ARocketGameModeBase * GameMode = Cast<ARocketGameModeBase>(GM))
		{
			if (GetPlayerState<ARocketPlayerState>())
			{
				GameMode->SpawnStart(this);
			}	
		}
	}
}

void ARocketPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	DestroyPlayerUI();
}

void ARocketPlayerController::StartUpdatingDeathSpec_Implementation(FVector DeathLocation)
{
	if (IsLocalController())
	{
		BlueprintRespawnTimer();

		if (ARocketPlayer* pRocketPawn = Cast<ARocketPlayer>(GetPawn()))
		{
			pRocketPawn->bCanMove = false;

			FRotator LookAtPlayer = UKismetMathLibrary::FindLookAtRotation(DeathSpecView->GetActorLocation(), pRocketPawn->GetActorLocation());
			DeathSpecView->SetActorRotation(LookAtPlayer);
		}

		//PlayerUI->SetRenderOpacity(0.0f);
		//DisableInput(this);
		if (DeathSpecView != nullptr)
		{
			DeathSpecView->SetActorLocation(DeathLocation);
			bShouldDeathSpecRotUpdate = true;

			this->SetViewTarget(DeathSpecView);
		}
	}
}

void ARocketPlayerController::StopUpdatingDeathSpec_Implementation()
{
	if (IsLocalController())
	{
		if (ARocketPlayer* pRocketPlayer = Cast<ARocketPlayer>(GetPawn()))
		{
			pRocketPlayer->bCanMove = true;
			this->SetViewTarget(GetPawn());

			bShouldDeathSpecRotUpdate = false;
			/*if (PlayerUI && bGameOver == false)
			{
				PlayerUI->SetRenderOpacity(1.0f);
			}*/
		}
	}
}

// Called to bind functionality to input
void ARocketPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction(TEXT("InGameMenu"), EInputEvent::IE_Pressed, this, &ARocketPlayerController::ToggleInGameMenu);
}

void ARocketPlayerController::CreatePlayerUI()
{
	if (IsLocalPlayerController() == true)
	{
		PlayerUI = CreateWidget(this, PlayerUIClass);
		if (PlayerUI == nullptr) return;
		PlayerUI->AddToViewport();
		PlayerUI->SetRenderOpacity(1.0f);

		if (InGameMenu == nullptr)
		{
			InGameMenu = CreateWidget(this, InGameMenuClass);
		}

	}
}

void ARocketPlayerController::DestroyPlayerUI()
{
	if (PlayerUI != nullptr)
	{
		PlayerUI->RemoveFromParent();
	}

	if (InGameMenu != nullptr)
	{
		InGameMenu->RemoveFromParent();
	}

	if (WinScreen != nullptr)
	{
		WinScreen->RemoveFromParent();
	}

	if (LoseScreen != nullptr)
	{
		LoseScreen->RemoveFromParent();
	}
}

void ARocketPlayerController::ToggleInGameMenu()
{
	if (InGameMenu != nullptr && bGameOver == false)
	{
		if (InGameMenu->IsInViewport() == false)
		{
			InGameMenu->AddToViewport();

			FInputModeGameAndUI InputData;
			//FInputModeUIOnly InputData;
			SetInputMode(InputData);
			bShowMouseCursor = true;
		}
		else
		{
			FInputModeGameOnly InputData;
			SetInputMode(InputData);
			InGameMenu->RemoveFromParent();
			bShowMouseCursor = false;
		}
	}
}

void ARocketPlayerController::CreateWinOrLoseScreen()
{
	bGameOver = true;

	if (InGameMenu->IsInViewport() == true)
	{
		FInputModeGameOnly InputData;
		SetInputMode(InputData);
		InGameMenu->RemoveFromParent();
		bShowMouseCursor = false;
	}

	PlayerUI->SetRenderOpacity(0.0f);

	UE_LOG(LogTemp, Warning, TEXT("CreateWinOrLoseScreen"));
	if (bIsWinner == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("Winner"));
		WinScreen = CreateWidget(this, WinScreenClass);
		if (WinScreen == nullptr) return;
		WinScreen->AddToViewport();
	}
	else if (bIsWinner == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Loser"));
		LoseScreen = CreateWidget(this, LoseScreenClass);
		if (LoseScreen == nullptr) return;
		LoseScreen->AddToViewport();
	}
}

void ARocketPlayerController::RPCSetControlRotation_Implementation(FRotator controllerRotation)
{
	RPCSetControlRotationMulti(controllerRotation);
}

void ARocketPlayerController::RPCSetControlRotationMulti_Implementation(FRotator controllerRotation)
{
	if (IsLocalController())
	{
		ControlRotation = controllerRotation;
	}
}

void ARocketPlayerController::RPCActivateMovement_Implementation()
{
	RPCActivateMovementMulti();
}

void ARocketPlayerController::RPCActivateMovementMulti_Implementation()
{
	if (IsLocalController())
	{
		if (ARocketPlayer* RocketPlayer = Cast<ARocketPlayer>(GetPawn()))
		{
			RocketPlayer->bPendingStartMove = true;
		}
	}
}

void ARocketPlayerController::BlueprintRespawnTimer_Implementation()
{

}
