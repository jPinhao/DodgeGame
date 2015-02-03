// Fill out your copyright notice in the Description page of Project Settings.

#include "Dodge.h"
#include "PelletSpawner.h"
#include "DodgeGameMode.h"
#include "SpawnController.h"
#include "Kismet/GameplayStatics.h"

APelletSpawner::APelletSpawner(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	spriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("Sprite"));
	if (spriteComponent)
	{
		SetRootComponent(spriteComponent);
	}
}

void APelletSpawner::BeginDestroy()
{
	//unregister ourselves from the gameMode
	UWorld *world = GetWorld();
	if (world)
	{
		AGameMode *gameMode = UGameplayStatics::GetGameMode(world);
		if (gameMode)
		{
			ADodgeGameMode *castGameMode = Cast<ADodgeGameMode>(gameMode);
			if (castGameMode)
			{
				ASpawnController *controller = castGameMode->GetSpawnController();
				controller->UnRegisterSpawner(this);
			}
		}
	}

	Super::BeginDestroy();
}

void APelletSpawner::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	spawnAreaComponent = Cast<UBoxComponent>(GetComponentByClass(UBoxComponent::StaticClass()));
}

void APelletSpawner::BeginPlay()
{
	Super::BeginPlay();
	//resize spawnArea to fit within other PelletSpawners and force spawning inside the box
	FVector extent, centre, rotatedExtent;

	//get the current component centre and extent
	extent = spawnAreaComponent->GetPlacementExtent().BoxExtent * spawnAreaComponent->GetComponentScale();
	centre = spawnAreaComponent->GetComponentLocation();
	//transform the extent to get a vector with the current component orientation (keep only X component, that's the only relevant scaled axis)
	rotatedExtent = spawnAreaComponent->GetComponentQuat().RotateVector(extent*FVector(1, 0, 0));

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
	extent.X = spawnAreaComponent->GetComponentQuat().Inverse().RotateVector(rotatedExtent).X;
	//and use the new extent to calculate the new scale
	spawnAreaComponent->SetWorldScale3D(extent / spawnAreaComponent->GetPlacementExtent().BoxExtent);
	spawnAreaComponent->SetWorldLocation(centre);

	//register ourselves into the spawncontroller
	if (bCanEverSpawn)
	{
		ADodgeGameMode *gameMode = Cast<ADodgeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (gameMode)
		{
			ASpawnController *controller = gameMode->GetSpawnController();
			controller->RegisterSpawner(this);
		}
	}
}

void APelletSpawner::SpawnPellet()
{
	FVector centre, extent, spawnPoint, thrustVector;
	FRotator thrustRotation;
	float spawnAngle = FMath::RandRange(10, 170);
	float x = 0.f, y = 0.f, z = 0.f;

	extent = spawnAreaComponent->GetPlacementExtent().BoxExtent * spawnAreaComponent->GetComponentScale();
	centre = spawnAreaComponent->GetComponentLocation();

	//randomize X position; make Z the edge of the component so the pellets spawn outside 
	x = FMath::RandRange(-extent.X, extent.X);
	z = spawnDirection[0].Z * extent.Z; 
	//apply component's rotation to our desired spawn point and add to centre point
	spawnPoint = centre + spawnAreaComponent->GetComponentQuat().RotateVector(FVector(x, y, z));
	//apply component's rotation + our random angle to get thrust angle
	thrustRotation = FRotator(spawnAreaComponent->GetComponentQuat() * FQuat(FVector(0, 1, 0), FMath::DegreesToRadians(spawnAngle)));
	thrustVector = thrustRotation.Vector();

	//thrustVector = GetWorld()->GetFirstPlayerController()->GetControlledPawn()->GetActorLocation() - spawnPoint;

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