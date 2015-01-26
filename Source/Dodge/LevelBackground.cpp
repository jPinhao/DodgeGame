// Fill out your copyright notice in the Description page of Project Settings.

#include "Dodge.h"
#include "LevelBackground.h"
#include "DodgeGameMode.h"
#include "Kismet/GameplayStatics.h"


ALevelBackground::ALevelBackground(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UBlueprint> BackgroundWidget(TEXT("Blueprint'/Game/BluePrints/BP_BackgroundWidget.BP_BackgroundWidget'"));
	if (BackgroundWidget.Object)
	{
		backgroundWidgetClass = (UClass*)BackgroundWidget.Object->GeneratedClass;
	}
	static ConstructorHelpers::FObjectFinder<UBlueprint> ScoreWidget(TEXT("Blueprint'/Game/BluePrints/BP_GameHUD.BP_GameHUD'"));
	if (ScoreWidget.Object)
	{
		scoreWidgetClass = (UClass*)ScoreWidget.Object->GeneratedClass;
	}
}

void ALevelBackground::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UActorComponent *box = GetComponentByClass(UBoxComponent::StaticClass());
	if (box)
	{
		blockingVolume = Cast<UBoxComponent>(box);
	}
	
	TArray<UActorComponent*> widgetComponents = GetComponentsByClass(UWidgetComponent::StaticClass());
	UWidgetComponent *widgetCom;
	for (int i = 0; i < widgetComponents.Num(); i++)
	{
		widgetCom = Cast<UWidgetComponent>(widgetComponents[i]);
		if (widgetCom->GetWidgetClass()->IsChildOf(backgroundWidgetClass))
		{
			background = widgetCom;
		}
		else if (widgetCom->GetWidgetClass()->IsChildOf(scoreWidgetClass))
		{
			score = widgetCom;
		}
	}

	OnInputTouchBegin.AddDynamic(this, &ALevelBackground::PressRestartLevel);
}

void ALevelBackground::BeginPlay()
{
	ResizeToNewResolution();
}

void ALevelBackground::ResizeToNewResolution()
{
	APlayerController *playerController = GetWorld()->GetFirstPlayerController();
	if (playerController)
	{
		ACameraActor *cameraActor = Cast<ACameraActor>(playerController->PlayerCameraManager->GetViewTarget());
		if (cameraActor)
		{
			UCameraComponent *camera = cameraActor->GetCameraComponent();
			if (camera)
			{
				float bgExtentH, bgExtentV;
				FVector newScale;

				bgExtentH = camera->OrthoWidth;
				bgExtentV = bgExtentH / camera->AspectRatio;

				newScale = FVector(bgExtentH, bgExtentV, 0.1f);
				newScale /= RootComponent->GetPlacementExtent().BoxExtent * 2;

				RootComponent->SetWorldScale3D(newScale);
				SetActorLocation(FVector(300.f, -bgExtentH / 2, -bgExtentV / 2));
			}
		}
	}
}

void ALevelBackground::PressRestartLevel(ETouchIndex::Type finger)
{
	UWorld *world = GetWorld();
	if (world)
	{
		AGameMode *gameMode = UGameplayStatics::GetGameMode(world);
		if (gameMode)
		{
			ADodgeGameMode *castGameMode = Cast<ADodgeGameMode>(gameMode);
			if (castGameMode)
			{
				if (castGameMode->GetGameState() == EPlayState::EGameOver)
				{
					castGameMode->ChangeGameState(EPlayState::EPlaying);
				}
			}
		}
	}
}