// Fill out your copyright notice in the Description page of Project Settings.

#include "Dodge.h"
#include "SpawnController.h"

ASpawnController::ASpawnController(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
{
	allLevelSpawners = TArray<APelletSpawner *>();
	PrimaryActorTick.bCanEverTick = true;
	EnableSpawning(false);
}

void ASpawnController::Tick(float deltaSeconds)
{
	SpawnNext(deltaSeconds);
}

void ASpawnController::EnableSpawning(bool enable)
{
	bSpawningEnabled = enable;
	SetActorTickEnabled(enable);
	if (enable) nextSpawn = 0.25;
}

void ASpawnController::SpawnNext(float deltaTime)
{
	if (!bSpawningEnabled) return;
	
	bool initialSpawn = false;
	if (nextSpawn == 0) initialSpawn = true;

	nextSpawn -= deltaTime;
	if (nextSpawn <= 0)
	{
		nextSpawn = spawnRate;
		/*if (initialSpawn)
		{
			for (auto spawner : allLevelSpawners)
			{
				spawner->SpawnPellet();
			}
		}
		else*/
		{
			short randomSpawner = FMath::RandRange(0, allLevelSpawners.Num() - 1);
			APelletSpawner *spawner = allLevelSpawners[randomSpawner];
			if (spawner)
			{
				spawner->SpawnPellet();
			}
		}		
	}
}

void ASpawnController::RegisterSpawner(APelletSpawner * const spawner)
{
	if (spawner) allLevelSpawners.AddUnique(spawner);
}

void ASpawnController::UnRegisterSpawner(APelletSpawner * const spawner)
{
	if (spawner) allLevelSpawners.RemoveSingle(spawner);
}


