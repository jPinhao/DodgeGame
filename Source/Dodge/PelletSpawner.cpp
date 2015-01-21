// Fill out your copyright notice in the Description page of Project Settings.

#include "Dodge.h"
#include "PelletSpawner.h"
#include "DodgeGameMode.h"

APelletSpawner::APelletSpawner(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	spriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("Sprite"));
	if (spriteComponent)
	{
		SetRootComponent(spriteComponent);
	}

	PrimaryActorTick.bCanEverTick = true;
	EnableSpawning(false);
}

void APelletSpawner::EnableSpawning(bool enable)
{
	spawningEnabled = enable;
	if (spawningEnabled)
	{
		nextSpawn = 0;
		SetActorTickEnabled(true);
	}
	else SetActorTickEnabled(false);
}

void APelletSpawner::Tick(float deltaTime)
{
	if (spawningEnabled == false) return;
	nextSpawn -= deltaTime;
	if (nextSpawn <= 0)
	{
		nextSpawn = spawnRate;
		FVector origin, extent, spawnPoint, spawnDirection;
		GetActorBounds(false, origin, extent);
		//spawn angle, 170 degrees, 5-175
		float spawnAngle = FMath::Rand() % 170 + 5;
		float x = 0.f, y = 0.f, z = 0.f;
		if (extent != FVector::ZeroVector)
		{
			switch (FMath::RandRange(0, 3))
			{
			case 0:
				y = origin.Y - extent.Y;
				z = (FMath::Rand() % (int)(2 * extent.Z)) + origin.Z - extent.Z;
				spawnAngle -= 90;
				break;
			case 1:
				y = (FMath::Rand() % (int)(2 * extent.Y)) + origin.Y - extent.Y;
				z = origin.Z + extent.Z;
				spawnAngle += 180;
				break;
			case 2:
				y = origin.Y + extent.Y;
				z = (FMath::Rand() % (int)(2 * extent.Z)) + origin.Z - extent.Z;
				spawnAngle += 90;
				break;
			case 3:
				y = (FMath::Rand() % (int)(2 * extent.Y)) + origin.Y - extent.Y;
				z = origin.Z - extent.Z;
				break;
			}
			//spawnPoint = FVector(0.f, origin.Y - extent.Y, origin.Z);
			//spawnAngle = 0.f;
			spawnPoint = FVector(x, y, z);
			spawnAngle = FMath::DegreesToRadians(spawnAngle);
			spawnDirection = FVector(0.f, FMath::Cos(spawnAngle), FMath::Sin(spawnAngle));
			APellet *newPellet = GetWorld()->SpawnActor<APellet>(spawnUnits->GetDefaultObject()->GetClass(), spawnPoint, FRotator(0.f, 0.f, 0.f));
			if (newPellet)
			{
				newPellet->SpawnDefaultController();
				newPellet->SetMovementDirection(spawnDirection);

				ADodgeGameMode *gameMode = Cast<ADodgeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
				if (gameMode)
				{
					gameMode->IncrementScoreTotalEnemyCount();
				}
			}
		}
	}
}