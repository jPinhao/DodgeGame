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

void APelletSpawner::BeginPlay()
{
	Super::BeginPlay();
	spawnAreaComponent = Cast<UBoxComponent>(GetComponentByClass(UBoxComponent::StaticClass()));
	//resize spawnArea to fit within other PelletSpawners and force spawning inside the box
	FVector extent, centre, rotatedExtent;

	//get the current component centre and extent
	extent = spawnAreaComponent->GetPlacementExtent().BoxExtent * spawnAreaComponent->GetComponentScale();
	centre = spawnAreaComponent->GetComponentLocation();
	//transform the extent to get a vector with the current component orientation (keep only Z component, that's the only relevant scaled axis)
	rotatedExtent = spawnAreaComponent->GetComponentQuat().RotateVector(extent*FVector(0, 0, 1));

	//check for colisions between spawn component centre and extremeties
	FVector posEdge = centre + rotatedExtent, negEdge = centre - rotatedExtent;
	FHitResult hitResult1 = FHitResult(), hitResult2 = FHitResult();
	FCollisionQueryParams collisionParams = FCollisionQueryParams();
	collisionParams.AddIgnoredActor(this);
	GetWorld()->LineTraceSingle(hitResult1, centre, posEdge, ECollisionChannel::ECC_WorldStatic, collisionParams);
	GetWorld()->LineTraceSingle(hitResult2, centre, negEdge, ECollisionChannel::ECC_WorldStatic, collisionParams);
	
	//if no colisions found, maintain original extremeties
	if (hitResult1.ImpactPoint != FVector::ZeroVector) posEdge=hitResult1.ImpactPoint;
	if (hitResult2.ImpactPoint != FVector::ZeroVector) negEdge=hitResult2.ImpactPoint;

	//desired new extent and component centre
	rotatedExtent = (posEdge - negEdge) / 2;
	centre = posEdge - rotatedExtent;
	extent.Z = spawnAreaComponent->GetComponentQuat().Inverse().RotateVector(rotatedExtent).Z;
	//and use the new extent to calculate the new scale
	spawnAreaComponent->SetWorldScale3D(extent / spawnAreaComponent->GetPlacementExtent().BoxExtent);
	spawnAreaComponent->SetWorldLocation(centre);
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

		FVector centre, extent, spawnPoint, thrustVector;
		FRotator thrustRotation;
		float spawnAngle = FMath::RandRange(-80, 80);
		float x = 0.f, y = 0.f, z = 0.f;

		extent = spawnAreaComponent->GetPlacementExtent().BoxExtent * spawnAreaComponent->GetComponentScale();
		centre = spawnAreaComponent->GetComponentLocation();

		//randomize Z position; make X/Y the edge of the component so the pellets spawn outside 
		z = FMath::RandRange(-extent.Z, extent.Z);
		x = spawnDirection[0].X * extent.X;
		y = spawnDirection[0].Y * extent.Y;
		//apply component's rotation to our desired spawn point and add to centre point
		spawnPoint = centre + spawnAreaComponent->GetComponentQuat().RotateVector(FVector(x, y, z));
		//apply component's rotation + our random angle to get thrust angle
		thrustRotation = FRotator(spawnAreaComponent->GetComponentQuat() * FQuat(FVector(0, 1, 0), FMath::DegreesToRadians(spawnAngle)));
		thrustVector = thrustRotation.Vector();

		APellet *newPellet = GetWorld()->SpawnActor<APellet>(spawnUnits->GetDefaultObject()->GetClass(), spawnPoint, FRotator(0.f, 0.f, 0.f));
		if (newPellet)
		{
			newPellet->SpawnDefaultController();
			newPellet->SetMovementDirection(thrustVector);

			ADodgeGameMode *gameMode = Cast<ADodgeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
			if (gameMode)
			{
				gameMode->IncrementScoreTotalEnemyCount();
			}
		}
	}
}