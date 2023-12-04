// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Gameframework/RotatingMovementComponent.h"

// Sets default values
APuzzlePawn::APuzzlePawn()
{
	// Initialize components
	USceneComponent* RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = RootSceneComponent;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("PuzzleMesh");
	StaticMesh->SetupAttachment(RootComponent);

	PuzzleCamera = CreateDefaultSubobject<UCameraComponent>("Camera");
	PuzzleCamera->SetupAttachment(RootComponent);

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

void APuzzlePawn::Interact(APlayerCharacter* PC)
{
	UE_LOG(LogTemp, Warning, TEXT("Calling Interact override on Puzzle Pawn"));
}

void APuzzlePawn::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();
	if (GetController())
	{
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void APuzzlePawn::Rotate(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();
	if (StaticMesh)
	{
		// Determine the rotation amount, you might want to multiply these by a rotation speed factor
		float DeltaYaw = LookAxisValue.X;
		float DeltaPitch = LookAxisValue.Y;

		// Get the current rotation
		FRotator CurrentRotation = StaticMesh->GetRelativeRotation();

		// Modify the rotation based on the input
		CurrentRotation.Yaw += DeltaYaw;
		CurrentRotation.Pitch += DeltaPitch;

		// Set the new rotation
		StaticMesh->SetRelativeRotation(CurrentRotation);
	}
}


// Called to bind functionality to input
void APuzzlePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Rotate
		EnhancedInputComponent->BindAction(RotateAction, ETriggerEvent::Triggered, this, &APuzzlePawn::Rotate);

		// Look
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APuzzlePawn::Look);
	}
}
