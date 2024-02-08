// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "InTheShadows/Player/PlayerCharacter.h"
#include "InTheShadows/HUD/PlayerHUD.h"
#include "InTheShadows/GameInstance/Its_GameInstance.h"

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
	TargetRotation = FRotator(0.4, 90.4, 112.6);
	RotationTolerance = 3.f;

	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APuzzlePawn::BeginPlay()
{
	Super::BeginPlay();

	InteractableData = InstanceInteractableData;

	// Loading puzzle state
	if (UIts_GameInstance* GI = Cast<UIts_GameInstance>(GetGameInstance()))
	{
		if (StaticMesh)
		{
			PuzzleTransform = StaticMesh->GetComponentTransform();
			UE_LOG(LogTemp, Warning, TEXT("Puzzle transform before loading: %s"), *PuzzleTransform.ToString());

			CurrentPuzzleState = GI->GetPuzzleState(InteractableData.Name, PuzzleTransform);
			if (CurrentPuzzleState.bIsPuzzleSolved)
				StaticMesh->SetWorldTransform(PuzzleTransform);
			UE_LOG(LogTemp, Warning,
			       TEXT("Puzzle State: %d, Puzzle Sound: %d with name %s and transform %s loaded from constructor"),
			       CurrentPuzzleState.bIsPuzzleSolved, CurrentPuzzleState.HasPlayedSound,
			       *InteractableData.Name.ToString(), *PuzzleTransform.ToString());
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("StaticMesh is nullptr"));
	}

	// Floating timeline
	if (FloatingCurve && StaticMesh && bIsFloating)
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

	if (IsRotationValid(TargetRotation, RotationTolerance) && !CurrentPuzzleState.bIsPuzzleSolved)
	{
		CurrentPuzzleState.bIsPuzzleSolved = true;
		PlaySoundAndDestroyWall();
		SetPuzzleSolved();
	}
}

void APuzzlePawn::PlaySoundAndDestroyWall()
{
	if (CurrentPuzzleState.bIsPuzzleSolved && !CurrentPuzzleState.HasPlayedSound)
	{
		CurrentPuzzleState.HasPlayedSound = true;
		PlayPuzzleSolvedSound();
		DestroyWall();
	}
}

// PUZZLE STATE
void APuzzlePawn::SetPuzzleSolved()
{
	if (UIts_GameInstance* GI = Cast<UIts_GameInstance>(GetGameInstance()))
	{
		if (StaticMesh && bSavePuzzleState)
		{
			PuzzleTransform = StaticMesh->GetComponentTransform();
			GI->SetPuzzleState(InteractableData.Name, CurrentPuzzleState, PuzzleTransform);
			UE_LOG(LogTemp, Warning, TEXT("Puzzle State: %d, Puzzle Sound: %d, with name %s and transform %s saved"),
			       CurrentPuzzleState.bIsPuzzleSolved, CurrentPuzzleState.HasPlayedSound,
			       *InteractableData.Name.ToString(), *PuzzleTransform.ToString());
		}
	}
}

// Check Rotation
bool APuzzlePawn::IsRotationValid(const FRotator& TargetRot, float Tolerance) const
{
	FRotator CurrentRotation = StaticMesh->GetRelativeRotation();

	CurrentRotation.Normalize();
	FRotator NormalizedTargetRot = TargetRot;
	NormalizedTargetRot.Normalize();
	// CurrentRotation.Roll = FMath::Abs(CurrentRotation.Roll);
	// CurrentRotation.Yaw = FMath::Abs(CurrentRotation.Yaw);
	// CurrentRotation.Pitch = FMath::Abs(CurrentRotation.Pitch);

	return CurrentRotation.Equals(NormalizedTargetRot, Tolerance);
}

// TIMELINE EVENTS
void APuzzlePawn::HandleFloatingTimelineProgress(float Value)
{
	FVector NewLocation = GetActorLocation();
	NewLocation.Z = StartLocation + Value;
	SetActorLocation(NewLocation);
}

// INTERFACE OVERRIDE
void APuzzlePawn::BeginFocus()
{
	if (StaticMesh)
	{
		StaticMesh->SetRenderCustomDepth(true);
		bIsRollActive = false;
	}
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

void APuzzlePawn::StartPossessing()
{
	UE_LOG(LogTemp, Warning, TEXT("Calling StartInteract override on Puzzle Pawn"));
	if (PlayerRef)
	{
		PlayerRef->HUD->ShowInteractionWidget();
		GetWorld()->GetTimerManager().SetTimer(InteractTimerHandle, this, &APuzzlePawn::OnLongPressComplete,
		                                       LongPressDuration, false);
	}
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
	UE_LOG(LogTemp, Warning, TEXT("Calling OnLongPressComplete override on Puzzle Pawn"));
	if (AController* PC = GetController())
	{
		if (PlayerRef)
		{
			bIsUnposs = true;
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

// INPUT
void APuzzlePawn::Look(const FInputActionValue& Value)
{
	if (!bIsRollActive && !IsRotationValid(TargetRotation, RotationTolerance))
	{
		const FVector2D LookAxisValue = Value.Get<FVector2D>();
		if (StaticMesh)
		{
			const float DeltaYaw = LookAxisValue.X;
			const float DeltaPitch = LookAxisValue.Y;

			// Convert current rotation to a quaternion
			FQuat CurrentQuat = StaticMesh->GetRelativeRotation().Quaternion();

			// Create delta rotation quaternions for yaw (Z axis) and pitch (Y axis)
			FQuat DeltaQuatYaw(FRotator(0.f, 0.f, DeltaYaw));
			FQuat DeltaQuatPitch(FRotator(DeltaPitch, 0.f, 0.f));

			// Apply yaw and pitch rotations
			FQuat NewQuat = DeltaQuatPitch * DeltaQuatYaw * CurrentQuat;

			// Set the new rotation
			StaticMesh->SetRelativeRotation(NewQuat.Rotator());
		}
	}
}

void APuzzlePawn::Roll(const FInputActionValue& Value)
{
	if (!IsRotationValid(TargetRotation, RotationTolerance))
	{
		bIsRollActive = true;
		const float RollAxisValue = Value.Get<float>();
		if (StaticMesh)
		{
			const float DeltaRoll = RollAxisValue;

			// Convert current rotation to a quaternion
			FQuat CurrentQuat = StaticMesh->GetRelativeRotation().Quaternion();

			// Create delta rotation quaternion for roll (X axis)
			FQuat DeltaQuatRoll(FRotator(0.f, DeltaRoll, 0.f));

			// Apply roll rotation
			FQuat NewQuat = DeltaQuatRoll * CurrentQuat;

			// Set the new rotation
			StaticMesh->SetRelativeRotation(NewQuat.Rotator());
		}
	}
}

// Called to bind functionality to input
void APuzzlePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Look
		if (bEnableLook)
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APuzzlePawn::Look);

		// Roll
		if (bEnableRoll)
		{
			EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Triggered, this, &APuzzlePawn::Roll);
			EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Completed, this, &APuzzlePawn::StopRoll);
		}

		// Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this,
		                                   &APuzzlePawn::StartPossessing);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this,
		                                   &APuzzlePawn::EndInteract);
	}
}
