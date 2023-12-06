// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "InTheShadows/Player/PlayerCharacter.h"
#include "InTheShadows/HUD/PlayerHUD.h"

// Sets default values
APuzzlePawn::APuzzlePawn()
{
	// Initialize components
	USceneComponent* RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = RootSceneComponent;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("PuzzleMesh");
	StaticMesh->SetupAttachment(RootComponent);

	USceneComponent* CameraHolder = CreateDefaultSubobject<USceneComponent>("CameraHolder");
	CameraHolder->SetupAttachment(RootComponent);

	PuzzleCamera = CreateDefaultSubobject<UCameraComponent>("Camera");
	PuzzleCamera->SetupAttachment(CameraHolder);
	PuzzleCamera->bUsePawnControlRotation = false;

	// Initialize variables
	bIsFloating = true;
	TargetRotation = FRotator(-1.60, 95.77, 118.17);
	RotationTolerance = 10.f;

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

	// Debug
	TimeSinceLastLog += DeltaTime;
	if (TimeSinceLastLog >= 5.0f)
	{
		FRotator CurrentRotation = StaticMesh->GetRelativeRotation();
		UE_LOG(LogTemp, Warning, TEXT("Current Rotation: Pitch=%f, Yaw=%f, Roll=%f"),
		       CurrentRotation.Pitch, CurrentRotation.Yaw, CurrentRotation.Roll);
		TimeSinceLastLog = 0.0f;
	}

	if (IsRotationValid(TargetRotation, RotationTolerance))
		UE_LOG(LogTemp, Warning, TEXT("!!! ROTATION IS VALID !!!"));
}

void APuzzlePawn::HandleFloatingTimelineProgress(float Value)
{
	FVector NewLocation = GetActorLocation();
	NewLocation.Z = StartLocation + Value;
	SetActorLocation(NewLocation);
}

// Check Rotation
bool APuzzlePawn::IsRotationValid(const FRotator& TargetRot, float Tolerance) const
{
	FRotator CurrentRotation = StaticMesh->GetRelativeRotation();
	return CurrentRotation.Equals(TargetRotation, Tolerance);
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
	UE_LOG(LogTemp, Warning, TEXT("Calling BeginInteract override on Puzzle Pawn"));
}

void APuzzlePawn::StartInteract()
{
	// Start a timer for long press detection
	UE_LOG(LogTemp, Warning, TEXT("Calling StartInteract override on Puzzle Pawn"));
	PlayerRef->HUD->ShowInteractionWidget();
	GetWorld()->GetTimerManager().SetTimer(InteractTimerHandle, this, &APuzzlePawn::OnLongPressComplete,
	                                       LongPressDuration, false);
}

void APuzzlePawn::EndInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("Calling EndInteract override on Puzzle Pawn"));
	if (GetWorld()->GetTimerManager().IsTimerActive(InteractTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(InteractTimerHandle);
	}
}

void APuzzlePawn::OnLongPressComplete()
{
	// PlayerRef = PC;
	UE_LOG(LogTemp, Warning, TEXT("Calling OnLongPressComplete override on Puzzle Pawn"));
	if (AController* PC = GetController())
	{
		if (PlayerRef)
		{
			PC->UnPossess();
			PC->Possess(PlayerRef);
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("PlayerRef is nullptr"));
	}
}

void APuzzlePawn::Interact(APlayerCharacter* PC)
{
	UE_LOG(LogTemp, Warning, TEXT("Calling Interact override on Puzzle Pawn"));
}

void APuzzlePawn::Look(const FInputActionValue& Value)
{
	if (!bIsRollActive)
	{
		const FVector2D LookAxisValue = Value.Get<FVector2D>();
		if (StaticMesh)
		{
			const float DeltaYaw = LookAxisValue.X;
			const float DeltaPitch = LookAxisValue.Y;

			FRotator CurrentRotation = StaticMesh->GetRelativeRotation();

			CurrentRotation.Yaw += DeltaYaw;
			CurrentRotation.Pitch += DeltaPitch;

			StaticMesh->SetRelativeRotation(CurrentRotation);
		}
	}
}

void APuzzlePawn::Roll(const FInputActionValue& Value)
{
	bIsRollActive = true;
	const float RollAxisValue = Value.Get<float>();
	if (StaticMesh)
	{
		const float DeltaRoll = RollAxisValue;

		FRotator CurrentRotation = StaticMesh->GetRelativeRotation();

		CurrentRotation.Roll += DeltaRoll;

		StaticMesh->SetRelativeRotation(CurrentRotation);
	}
}

// Called to bind functionality to input
void APuzzlePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Look
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APuzzlePawn::Look);

		// Roll
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Triggered, this, &APuzzlePawn::Roll);
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Completed, this, &APuzzlePawn::StopRoll);

		// Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this,
		                                   &APuzzlePawn::StartInteract);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this,
		                                   &APuzzlePawn::EndInteract);
	}
}
