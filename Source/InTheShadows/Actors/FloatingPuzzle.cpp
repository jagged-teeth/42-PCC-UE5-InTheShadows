// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingPuzzle.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Gameframework/RotatingMovementComponent.h"

// Sets default values
AFloatingPuzzle::AFloatingPuzzle()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("PuzzleMesh");
	RootComponent = StaticMesh;

	SphereCollider = CreateDefaultSubobject<USphereComponent>("Sphere");
	SphereCollider->SetupAttachment(RootComponent);
	SphereCollider->InitSphereRadius(64.f);
	SphereCollider->SetCollisionProfileName(TEXT("Trigger"));

	RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>("RotatingMovement");

	bIsFloating = true;

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AFloatingPuzzle::BeginPlay()
{
	Super::BeginPlay();

	InteractableData = InstanceInteractableData;

	if (FloatingCurve && bIsFloating)
	{
		StartLocation = GetActorLocation().Z;

		FOnTimelineFloat ProgressFunction;
		ProgressFunction.BindUFunction(this, FName("HandleFloatingTimelineProgress"));
		FloatingTimeline.AddInterpFloat(FloatingCurve, ProgressFunction);

		FloatingTimeline.SetLooping(true);
		FloatingTimeline.PlayFromStart();
	}
}

// Called every frame
void AFloatingPuzzle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FloatingTimeline.TickTimeline(DeltaTime);
}

void AFloatingPuzzle::HandleFloatingTimelineProgress(float Value)
{
	FVector NewLocation = GetActorLocation();
	NewLocation.Z = StartLocation + Value;
	SetActorLocation(NewLocation);
}

void AFloatingPuzzle::BeginFocus()
{
	if (StaticMesh)
		StaticMesh->SetRenderCustomDepth(true);
}

void AFloatingPuzzle::EndFocus()
{
	if (StaticMesh)
		StaticMesh->SetRenderCustomDepth(false);
	if (IsInteracting)
		EndInteract();
}

void AFloatingPuzzle::BeginInteract()
{
	IsInteracting = true;
	UE_LOG(LogTemp, Warning, TEXT("Calling BeginInteract override on FloatingPuzzle actor"));
}

void AFloatingPuzzle::EndInteract()
{
	IsInteracting = false;
	UE_LOG(LogTemp, Warning, TEXT("Calling EndInteract override on FloatingPuzzle actor"));
}

void AFloatingPuzzle::Interact(APlayerCharacter* PC)
{
	UE_LOG(LogTemp, Warning, TEXT("Calling Interact override on FloatingPuzzle actor"));
}

void AFloatingPuzzle::SetPuzzleControlActive(bool bIsActive)
{
	if (bIsActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("Setting PuzzleControlActive to true"));
		bIsControlledByPlayer = true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Setting PuzzleControlActive to false"));
		bIsControlledByPlayer = false;
	}
}

// void AFloatingPuzzle::RotatePuzzle(FInputActionValue& Value)
// {
// 	if (bIsControlledByPlayer)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("Rotating Puzzle"));
// 		
// 	}
// }