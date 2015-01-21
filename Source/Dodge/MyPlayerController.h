// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class DODGE_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void Tick(float deltaTime) override;

private:
	bool AcquireTarget(FVector &out_target);
	
};
