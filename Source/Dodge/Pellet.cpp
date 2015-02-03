// Fill out your copyright notice in the Description page of Project Settings.

#include "Dodge.h"
#include "Pellet.h"
#include "DodgeGameMode.h"
#include "Kismet/GameplayStatics.h"

APellet::APellet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	movementDirection = FVector::ZeroVector;
	bFollowPointer = false;
	if (moveSpeed == 0) moveSpeed = 1.f;
	PawnMovement = ObjectInitializer.CreateDefaultSubobject<UFloatingPawnMovement>(this, TEXT("MovementComponent"));
}

/** after all game elements are created */
void APellet::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (PawnMovement)
	{
		DEBUG_ERROR(FString::SanitizeFloat(moveSpeed));
		PawnMovement->Activate();
		PawnMovement->MaxSpeed = moveSpeed;
	}

	colisionComponent = Cast<UPrimitiveComponent>(FindComponentByClass(USphereComponent::StaticClass()));
	if (colisionComponent)
	{
		colisionComponent->OnComponentHit.AddDynamic(this, &APellet::OnColide);
	}

	spriteComponent = Cast<UPaperSpriteComponent>(FindComponentByClass(UPaperSpriteComponent::StaticClass()));

	if (spawnAnimation)
	{
		animComponent = Cast<UPaperFlipbookComponent>(FindComponentByClass(UPaperFlipbookComponent::StaticClass()));
	}
}

void APellet::BeginPlay()
{
	Super::BeginPlay();
	BeginSpawn();
}

void APellet::BeginSpawn()
{
	if (spawnAnimation && animComponent)
	{
		PrepareAnimation(spawnAnimation, false);
		animComponent->OnFinishedPlaying.AddDynamic(this, &APellet::FinishSpawn);
		if (colisionComponent) colisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else FinishSpawn();
}

void APellet::FinishSpawn()
{
	if (colisionComponent)
	{
		colisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}


void APellet::PrepareAnimation(UPaperFlipbook *animation, bool loop)
{
	if (animComponent)
	{
		animComponent->SetFlipbook(animation);
		animComponent->SetVisibility(true);
		animComponent->SetLooping(loop);
		if (!loop)
		{
			animComponent->OnFinishedPlaying.AddDynamic(this, &APellet::FinishAnimation);
		}
		if (spriteComponent) spriteComponent->SetVisibility(false);
	}
}

void APellet::FinishAnimation()
{
	if (animComponent)
	{
		animComponent->Stop();
		animComponent->SetVisibility(false);
		animComponent->SetFlipbook(nullptr);
		animComponent->OnFinishedPlaying.Clear();
		if (spriteComponent) spriteComponent->SetVisibility(true);
	}
}

void APellet::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	InputComponent->BindAction("FollowPointer", IE_Pressed, this, &APellet::StartTrackingPointer);
	InputComponent->BindAction("FollowPointer", IE_Repeat, this, &APellet::StartTrackingPointer);
	InputComponent->BindAction("FollowPointer", IE_Released, this, &APellet::StopTrackingPointer);

	InputComponent->BindTouch(IE_Pressed, this, &APellet::StartTrackingPointerTouch);
	InputComponent->BindTouch(IE_Repeat, this, &APellet::StartTrackingPointerTouch);
	InputComponent->BindTouch(IE_Released, this, &APellet::StopTrackingPointerTouch);
}

void APellet::StartTrackingPointer()
{
	bFollowPointer = true;
}

void APellet::StopTrackingPointer()
{
	bFollowPointer = false;
	movementDirection = FVector::ZeroVector;
}

void APellet::StartTrackingPointerTouch(ETouchIndex::Type finger, FVector position)
{
	StartTrackingPointer();
	touchPosition = position;
}

void APellet::StopTrackingPointerTouch(ETouchIndex::Type finger, FVector position)
{
	StopTrackingPointer();
}

void APellet::Tick(float deltaTime)
{
	if (!IsSpawning())
	{
		ChaseTarget(deltaTime);
	}
}

void APellet::ChaseTarget(float deltaTime)
{
	SetActorLocation(GetActorLocation()*FVector(0, 1, 1));
	AddMovementInput(movementDirection, moveSpeed);
}
	
void APellet::OnColide(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor->IsPendingKill() && OtherActor->GetActorClass() == GetActorClass())
	{
		APellet *otherPellet = (APellet*)OtherActor;
		if (GetController()->IsA(APlayerController::StaticClass()))
		{
			DetachFromControllerPendingDestroy();
			Destroy();

			ADodgeGameMode *gameMode = Cast<ADodgeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
			if (gameMode)
			{
				gameMode->ChangeGameState(EPlayState::EPlayerDead);
			}
		}
		else
		{
			SetMovementDirection(CalcBounceDirection(movementDirection, Hit.ImpactNormal));
			//AddMovementInput(movementDirection, moveSpeed);
			//otherPellet->SetMovementDirection(CalcBounceDirection(-Hit.ImpactNormal, movementDirection));
			/*if (OtherActor->GetActorScale3D().Size() <= GetActorScale3D().Size())
			{
				SetActorScale3D((OtherActor->GetActorScale3D() + GetActorScale3D()));
				OtherActor->Destroy();
			}*/
		}
	}
	else
	{
		if (GetController() && !GetController()->IsA(APlayerController::StaticClass()))
		{
			SetMovementDirection(CalcBounceDirection(movementDirection, Hit.ImpactNormal));
			//calculate the new angle - half the angles between wall and current movement
			/*float dotProduct = FVector::DotProduct(movementDirection, otherActorDirection);
			float magnitudeSum = movementDirection.Size() + otherActorDirection.Size();
			float newMovementAngle = FMath::Acos(dotProduct / magnitudeSum);
			newMovementAngle = PI - newMovementAngle;*/
		}
	}
}

FVector APellet::CalcBounceDirection(const FVector &myDirection, const FVector &hitNormal)
{
	FVector moveComponetAlongNormal = (FVector::DotProduct(myDirection, hitNormal) / FVector::DotProduct(hitNormal, hitNormal))*hitNormal;
	FVector moveComponetPerpNormal = myDirection - moveComponetAlongNormal;
	return(moveComponetPerpNormal - moveComponetAlongNormal);
}

bool APellet::SetTargetPosition(const FVector &targetPos)
{
	SetMovementDirection(targetPos - GetActorLocation());
	return true;
}

bool APellet::SetMovementDirection(const FVector &movementDirection)
{
	this->movementDirection = movementDirection;
	this->movementDirection.X = 0;
	this->movementDirection.Normalize();
	if (IsSpawning())
	{
		PawnMovement->ConsumeInputVector();
		AddMovementInput(this->movementDirection, moveSpeed);
	}
	return true;
}

FVector& APellet::GetMovementDirection()
{
	return movementDirection;
}

bool APellet::CanChase()
{
	return bFollowPointer;
}

bool APellet::IsSpawning()
{
	if (spawnAnimation && animComponent)
	{
		if (animComponent->GetFlipbook() == spawnAnimation)
		{
			if(animComponent->IsPlaying()) return true;
		}
	}
	return false;
}