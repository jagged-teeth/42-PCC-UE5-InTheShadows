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
		PuzzleTransform = StaticMesh->GetComponentTransform();
		UE_LOG(LogTemp, Warning, TEXT("Puzzle transform before loading: %s"), *PuzzleTransform.ToString());
		
		bIsPuzzleSolved = GI->GetPuzzleState(InteractableData.Name, PuzzleTransform);
		if (bIsPuzzleSolved)
			StaticMesh->SetWorldTransform(PuzzleTransform);
		UE_LOG(LogTemp, Warning, TEXT("Puzzle State: %d, with name %s and transform %s loaded from constructor"),
		       bIsPuzzleSolved, *InteractableData.Name.ToString(), *PuzzleTransform.ToString());
	}

	// Floating timeline
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

	if (IsRotationValid(TargetRotation, RotationTolerance) && !bIsPuzzleSolved)
	{
		bIsPuzzleSolved = true;
		SetPuzzleSolved(bIsPuzzleSolved);
	}
}

// PUZZLE STATE
void APuzzlePawn::SetPuzzleSolved(bool Solved)
{
	bIsPuzzleSolved = Solved;
	UIts_GameInstance* GI = Cast<UIts_GameInstance>(GetGameInstance());
	if (GI)
	{
		PuzzleTransform = StaticMesh->GetComponentTransform();
		GI->SetPuzzleState(InteractableData.Name, bIsPuzzleSolved, PuzzleTransform);
		UE_LOG(LogTemp, Warning, TEXT("Puzzle State: %d, with name %s and transform %s saved"), bIsPuzzleSolved,
		       *InteractableData.Name.ToString(), *PuzzleTransform.ToString());
	}
}

// Check Rotation
bool APuzzlePawn::IsRotationValid(const FRotator& TargetRot, float Tolerance) const
{
	FRotator CurrentRotation = StaticMesh->GetRelativeRotation();

	CurrentRotation.Roll = FMath::Abs(CurrentRotation.Roll);
	CurrentRotation.Yaw = FMath::Abs(CurrentRotation.Yaw);
	CurrentRotation.Pitch = FMath::Abs(CurrentRotation.Pitch);

	return CurrentRotation.Equals(TargetRotation, Tolerance);
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
	UE_LOG(LogTemp, Warning, TEXT("Calling OnLongPressComplete override on Puzzle Pawn"));
	if (AController* PC = GetController())
	{
		if (PlayerRef)
		{
			bIsUnpossessing = true;
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

			FRotator CurrentRotation = StaticMesh->GetRelativeRotation();

			CurrentRotation.Yaw += DeltaYaw;
			CurrentRotation.Pitch += DeltaPitch;

			StaticMesh->SetRelativeRotation(CurrentRotation);
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

			FRotator CurrentRotation = StaticMesh->GetRelativeRotation();

			CurrentRotation.Roll += DeltaRoll;

			StaticMesh->SetRelativeRotation(CurrentRotation);
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
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APuzzlePawn::Look);

		// Roll
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Triggered, this, &APuzzlePawn::Roll);
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Completed, this, &APuzzlePawn::StopRoll);

		// Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this,
		                                   &APuzzlePawn::StartPossessing);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this,
		                                   &APuzzlePawn::EndInteract);
	}
}
