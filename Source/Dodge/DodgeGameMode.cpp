// Fill out your copyright notice in the Description page of Project Settings.

#include "Dodge.h"
#include "DodgeGameMode.h"
#include "PelletSpawner.h"
#include "Kismet/GameplayStatics.h"

ADodgeGameMode::ADodgeGameMode(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	/*static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnObject(TEXT("Blueprint'/Game/BluePrints/BP_Pellet.BP_Pellet'"));
	if (PlayerPawnObject.Object)
	{
		DefaultPawnClass = (UClass*)PlayerPawnObject.Object->GeneratedClass;
	}
	static ConstructorHelpers::FObjectFinder<UBlueprint> GameHUDObject(TEXT("Blueprint'/Game/BluePrints/BP_GameHUD.BP_GameHUD'"));
	if (GameHUDObject.Object)
	{
		HUDClass = (UClass*)GameHUDObject.Object->GeneratedClass;
	}*/
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
		EnablePelletSpawners(false);
		DespawnAll();
		DEBUG_INFO(FString(TEXT("Final Score: ")).Append(FString::FromInt(totalEnemies)));
	}
	else if (newGameState == EPlayState::EPlaying)
	{
		ResetGameState();
		EnablePelletSpawners(true);
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

void ADodgeGameMode::EnablePelletSpawners(bool enable)
{
	TArray<AActor*> allSpawners;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APelletSpawner::StaticClass(), allSpawners);
	for (auto spawner : allSpawners)
	{
		APelletSpawner *pelletSpawner = Cast<APelletSpawner>(spawner);
		if (pelletSpawner)
		{
			pelletSpawner->EnableSpawning(enable);
		}
	}
}