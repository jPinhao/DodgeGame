// Fill out your copyright notice in the Description page of Project Settings.

#include "Dodge.h"
#include "MyPlayerController.h"
#include "Pellet.h"

void AMyPlayerController::Tick(float deltaTime)
{
	APellet *myCharacter = (APellet*)GetPawn();
	if (myCharacter)
	{
		FVector targetPosition;
		if (AcquireTarget(targetPosition))
		{
			myCharacter->SetTargetPosition(targetPosition);
		}
	}
}

bool AMyPlayerController::AcquireTarget(FVector &out_target)
{
	APellet *myCharacter = (APellet*)GetPawn();
	if (myCharacter && myCharacter->CanChase())
	{
		FVector worldPos, worldDir;
		if (myCharacter->touchPosition != FVector::ZeroVector) DeprojectScreenPositionToWorld(myCharacter->touchPosition.X, myCharacter->touchPosition.Y, worldPos, worldDir);
		else DeprojectMousePositionToWorld(worldPos, worldDir);
		FHitResult mousePointerTrace;
		if (GetWorld()->LineTraceSingle(mousePointerTrace, worldPos, worldPos + worldDir * 10000, ECC_WorldStatic, FCollisionQueryParams()))
		{
			out_target = mousePointerTrace.ImpactPoint;
			return true;
		}
	}
	return false;
}


