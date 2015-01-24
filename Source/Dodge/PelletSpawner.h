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
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

	void SpawnPellet();

private:
	UPROPERTY(VisibleAnywhere, Category = ComponentStructure)
	UPaperSpriteComponent *spriteComponent;
	UBoxComponent *spawnAreaComponent;
	UPROPERTY(EditDefaultsOnly, Category = SpawnProperties)
	TSubclassOf<APellet> spawnUnits;
	UPROPERTY(EditInstanceOnly, Category = SpawnProperties)
	FVector spawnDirection[4];
	UPROPERTY(EditInstanceOnly, Category = SpawnProperties)
	bool bCanEverSpawn;
};
