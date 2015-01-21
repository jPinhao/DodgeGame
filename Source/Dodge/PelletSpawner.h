// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PaperSpriteComponent.h"
#include "Pellet.h"
#include "PelletSpawner.generated.h"

/**
 * 
 */
UCLASS()
class DODGE_API APelletSpawner : public AActor
{
	GENERATED_BODY()

public:
	APelletSpawner(const FObjectInitializer& ObjectInitializer);
	void Tick(float deltaTime) override;

	void EnableSpawning(bool enable);

	UPaperSpriteComponent *spriteComponent;

private:
	UPROPERTY(EditAnywhere, Category = SpawnProperties)
	float spawnRate;
	float nextSpawn=1.f;
	UPROPERTY(EditAnywhere, Category = SpawnProperties)
	TSubclassOf<APellet> spawnUnits;
	UPROPERTY(EditAnywhere, Category = SpawnProperties)
	bool spawningEnabled;
};
