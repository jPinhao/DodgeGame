// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "SpawnController.h"
#include "DodgeGameMode.generated.h"

UENUM(BlueprintType)
enum class EPlayState : uint8
{
	EPlaying UMETA(DisplayName = "Playing"),
	EGameOver UMETA(DisplayName = "GameOver"),
	EUnknown UMETA(DisplayName = "InvalidState")
};

/**
 * 
 */
UCLASS()
class DODGE_API ADodgeGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ADodgeGameMode(const FObjectInitializer& ObjectInitializer);
	virtual void StartPlay() override;

	void IncrementScoreTotalEnemyCount();
	UFUNCTION(BlueprintCallable, Category = GameState)
	void ChangeGameState(EPlayState newGameState);
	UFUNCTION(BlueprintCallable, Category = GameState)
	int32 GetTotalEnemies();
	UFUNCTION(BlueprintCallable, Category = GameState)
	EPlayState GetGameState();
	ASpawnController* GetSpawnController();

private:
	void ResetGameState();
	void DespawnAll();

	ASpawnController *spawnController;
	EPlayState gameState;
	UPROPERTY(VisibleAnywhere, Category = GameState)
	int32 totalEnemies;
};