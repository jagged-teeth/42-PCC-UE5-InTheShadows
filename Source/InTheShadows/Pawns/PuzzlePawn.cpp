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
	UE_LOG(LogTemp, Warning, TEXT("Calling BeginInteract override on Puzzle Pawn"));
	EndFocus();

	PlayerHUD = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (PlayerHUD)
		PlayerHUD->HideInteractionWidget();
}

void APuzzlePawn::StartInteract()
{
	// Start a timer for long press detection
	UE_LOG(LogTemp, Warning, TEXT("Calling StartInteract override on Puzzle Pawn"));
	PlayerHUD->ShowInteractionWidget();
	PlayerHUD->ShowProgressBar();
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
	if (AController* PController = GetController())
	{
		PController->UnPossess();
		// Controller->Possess(PlayerRef);
	}
}

void APuzzlePawn::Interact(APlayerCharacter* PC)
{
	UE_LOG(LogTemp, Warning, TEXT("Calling Interact override on Puzzle Pawn"));
}

void APuzzlePawn::Look(const FInputActionValue& Value)
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

// Called to bind functionality to input
void APuzzlePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Look
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APuzzlePawn::Look);

		// Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this,
		                                   &APuzzlePawn::StartInteract);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this,
		                                   &APuzzlePawn::EndInteract);
	}
}
