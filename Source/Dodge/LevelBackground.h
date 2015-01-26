// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/Components/WidgetComponent.h"
#include "LevelBackground.generated.h"




/**
 * 
 */
UCLASS()
class DODGE_API ALevelBackground : public AActor
{
	GENERATED_BODY()

public:
	ALevelBackground(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	
	void ResizeToNewResolution();

private :
	UWidgetComponent *background;
	UClass *backgroundWidgetClass;
	UWidgetComponent *score;
	UClass *scoreWidgetClass;
	UBoxComponent *blockingVolume;

	UFUNCTION()
	void PressRestartLevel(ETouchIndex::Type finger);
};
