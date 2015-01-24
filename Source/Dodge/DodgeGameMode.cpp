// Fill out your copyright notice in the Description page of Project Settings.

#include "Dodge.h"
#include "DodgeGameMode.h"
#include "PelletSpawner.h"
#include "Kismet/GameplayStatics.h"

ADodgeGameMode::ADodgeGameMode(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ADodgeGameMode::StartPlay()
{
	DEBUG_INFO(TEXT("ADodgeGameMode::StartPlay()"));
	Super::StartPlay();
	ChangeGameState(EPlayState::EPlaying);
}

void ADodgeGameMode::ChangeGameState(EPlayState newGameState)
{
	gameState = newGameState;
	if (newGameState == EPlayState::EGameOver)
	{
		GetSpawnController()->EnableSpawning(false);
		DespawnAll();
		DEBUG_INFO(FString(TEXT("Final Score: ")).Append(FString::FromInt(totalEnemies)));
	}
	else if (newGameState == EPlayState::EPlaying)
	{
		ResetGameState();
		GetSpawnController()->EnableSpawning(true);
		APlayerController *playerController = GetWorld()->GetFirstPlayerController();
		if (!playerController)
		{
			playerController = SpawnPlayerController(FVector(0, 0, 0), FRotator(0, 0, 0));
		}
		if (playerController && !playerController->GetPawn())
		{
			APawn* newPawn=SpawnDefaultPawnFor(playerController, PlayerStarts[0]);
			playerController->Possess(newPawn);
		}
	}
}

void ADodgeGameMode::IncrementScoreTotalEnemyCount()
{
	totalEnemies++;
}

void ADodgeGameMode::ResetGameState()
{
	totalEnemies = 0;
}

void ADodgeGameMode::DespawnAll()
{
	TArray<AActor*> allPawns;
	APellet *pellet;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), DefaultPawnClass, allPawns);
	for (auto pawn : allPawns)
	{
		if (!pawn->IsPendingKill())
		{
			pellet = Cast<APellet>(pawn);
			if (pellet)
			{
				pellet->DetachFromControllerPendingDestroy();
				pellet->Destroy();
			}
		}
	}
}

int32 ADodgeGameMode::GetTotalEnemies()
{
	return totalEnemies;
}

EPlayState ADodgeGameMode::GetGameState()
{
	return gameState;
}

ASpawnController* ADodgeGameMode::GetSpawnController()
{
	if (!spawnController)
	{
		TArray<AActor*> allSpawnControllers;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnController::StaticClass(), allSpawnControllers);
		for (int i = 0; i < allSpawnControllers.Num(); i++)
		{
			ASpawnController *castSpawnController = Cast<ASpawnController>(allSpawnControllers[i]);
			if (castSpawnController) spawnController = castSpawnController;
		}
	}
	return spawnController;
}