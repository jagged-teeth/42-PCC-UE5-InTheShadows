// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Gameframework/RotatingMovementComponent.h"

// Sets default values
APuzzlePawn::APuzzlePawn()
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
void APuzzlePawn::BeginPlay()
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
void APuzzlePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FloatingTimeline.TickTimeline(DeltaTime);
}

void APuzzlePawn::HandleFloatingTimelineProgress(float Value)
{
	FVector NewLocation = GetActorLocation();
	NewLocation.Z = StartLocation + Value;
	SetActorLocation(NewLocation);
}

void APuzzlePawn::BeginFocus()
{
	if (StaticMesh)
		StaticMesh->SetRenderCustomDepth(true);
}

void APuzzlePawn::EndFocus()
{
	if (StaticMesh)
		StaticMesh->SetRenderCustomDepth(false);
	if (IsInteracting)
		EndInteract();
}

void APuzzlePawn::BeginInteract()
{
	IsInteracting = true;
	UE_LOG(LogTemp, Warning, TEXT("Calling BeginInteract override on Puzzle Pawn"));
}

void APuzzlePawn::EndInteract()
{
	IsInteracting = false;
	UE_LOG(LogTemp, Warning, TEXT("Calling EndInteract override on Puzzle Pawn"));
}

void APuzzlePawn::Interact(APlayerCharacter* PC)
{
	UE_LOG(LogTemp, Warning, TEXT("Calling Interact override on Puzzle Pawn"));
	SetPuzzleControlActive(true);
}

void APuzzlePawn::SetPuzzleControlActive(bool bIsActive)
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

void APuzzlePawn::Move(const FInputActionValue& Value)
{
	const FVector2D MoveAxisValue = Value.Get<FVector2D>();
	if (GetController() && !MoveAxisValue.IsZero())
	{
		AddMovementInput(GetActorForwardVector(), MoveAxisValue.Y);
		AddMovementInput(GetActorRightVector(), MoveAxisValue.X);
	}
}

// Called to bind functionality to input
void APuzzlePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Movement
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APuzzlePawn::Move);
	}
}