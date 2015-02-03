// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "DodgeUMG.h"
#include "PaperSpriteComponent.h"
#include "PaperFlipbookComponent.h"
#include "Pellet.generated.h"

/**
 * 
 */
UCLASS()
class DODGE_API APellet : public APawn
{
	GENERATED_BODY()

public:
	APellet(const FObjectInitializer& ObjectInitializer);
	/* after all game elements are created */
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	void SetupPlayerInputComponent(class UInputComponent* InputComponent);
	
	void Tick(float deltaTime) override;
	void ChaseTarget(float deltaTime);
	UFUNCTION()
	void OnColide(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	bool SetTargetPosition(const FVector &targetPos);
	bool SetMovementDirection(const FVector &movementDirection);
	FVector& GetMovementDirection();
	bool CanChase();
	bool IsSpawning();

	UFloatingPawnMovement *PawnMovement;
	FVector touchPosition;
private:
	UPROPERTY(EditAnywhere, Category = BehaviourProperties)
	float moveSpeed;
	FVector movementDirection;

	//input 
	bool bFollowPointer;
	void StartTrackingPointer();
	void StopTrackingPointer();
	void StartTrackingPointerTouch(ETouchIndex::Type finger,FVector position);
	void StopTrackingPointerTouch(ETouchIndex::Type finger, FVector position);

	//spawning
	UPROPERTY(EditAnywhere, Category = SpawnProperties)
	UPaperFlipbook *spawnAnimation;
	void BeginSpawn();
	UFUNCTION()
	void FinishSpawn();

	//animation utilities
	void PrepareAnimation(UPaperFlipbook *animation, bool loop);
	UFUNCTION()
	void FinishAnimation();
	UPaperFlipbookComponent *animComponent;
	UPaperSpriteComponent *spriteComponent;


	//object utilities
	FVector CalcBounceDirection(const FVector &myDirection, const FVector &hitNormal);
	UPrimitiveComponent *colisionComponent;
};
