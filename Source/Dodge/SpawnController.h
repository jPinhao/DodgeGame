// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PelletSpawner.h"
#include "SpawnController.generated.h"

/**
 * 
 */
UCLASS()
class DODGE_API ASpawnController : public AActor
{
	GENERATED_BODY()

public:
	ASpawnController(const FObjectInitializer &ObjectInitializer);
	virtual void Tick(float deltaSeconds) override;

	void SpawnNext(float deltaTime);
	void EnableSpawning(bool enable);
	void RegisterSpawner(APelletSpawner * const spawner);
	void UnRegisterSpawner(APelletSpawner * const spawner);
	
private :
	TArray<APelletSpawner *> allLevelSpawners;
	UPROPERTY(EditAnywhere, Category = SpawnProperties)
	float spawnRate;
	float nextSpawn;
	UPROPERTY(EditAnywhere, Category = SpawnProperties)
	bool bSpawningEnabled;	
};
