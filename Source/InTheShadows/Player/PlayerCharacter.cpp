// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "InTheShadows/Pawns/PuzzlePawn.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include "InTheShadows/HUD/PlayerHUD.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setup the capsule component
	PlayerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlayerMesh"));
	PlayerMesh->SetupAttachment(GetCapsuleComponent());

	// Setup the camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(PlayerMesh);
	CameraBoom->TargetArmLength = 0.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Setup the interaction data
	InteractionCheckFrequency = 0.1f;
	InteractionCheckDistance = 250.f;
	BaseEyeHeight = 30.f;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	HUD = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		// PC->bShowMouseCursor = true;
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(BaseMappingContext, 1);
		}
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
		PerformInteractionCheck();
}

// Interaction
void APlayerCharacter::PerformInteractionCheck()
{
	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	FVector TraceStart{GetPawnViewLocation()};
	FVector TraceEnd{TraceStart + GetViewRotation().Vector() * InteractionCheckDistance};
	float TraceRadius = 50.f;

	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 1.0f);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	FHitResult HitResult;

	if (GetWorld()->SweepSingleByChannel(HitResult, TraceStart, TraceEnd, FQuat::Identity, ECC_Visibility,
	                                     FCollisionShape::MakeSphere(TraceRadius), QueryParams))
	{
		if (HitResult.GetActor()->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
		{
			if (HitResult.GetActor() != InteractionData.CurrentInteractable)
			{
				FoundInteractable(HitResult.GetActor());
				return;
			}
			if (HitResult.GetActor() == InteractionData.CurrentInteractable)
				return;
		}
	}
	LostInteractable();
}

void APlayerCharacter::FoundInteractable(AActor* NewInteractable)
{
	if (IsInteracting())
		EndInteract();

	if (InteractionData.CurrentInteractable)
	{
		// End the previous interactable focus
		TargetInteractable = InteractionData.CurrentInteractable;
		TargetInteractable->EndFocus();
	}

	InteractionData.CurrentInteractable = NewInteractable;
	TargetInteractable = NewInteractable;

	HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);

	TargetInteractable->BeginFocus();
}

void APlayerCharacter::LostInteractable()
{
	if (IsInteracting())
		GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);

	if (InteractionData.CurrentInteractable)
	{
		if (IsValid(TargetInteractable.GetObject()))
			TargetInteractable->EndFocus();

		HUD->HideInteractionWidget();

		InteractionData.CurrentInteractable = nullptr;
		TargetInteractable = nullptr;
	}
}

void APlayerCharacter::BeginInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("Calling BeginInteract override on PlayerCharacter actor"));
	// Verify nothing has changed with the interactable state since beginning of the interaction
	PerformInteractionCheck();

	if (InteractionData.CurrentInteractable)
	{
		if (IsValid(TargetInteractable.GetObject()))
		{
			TargetInteractable->BeginInteract();

			if (FMath::IsNearlyZero(TargetInteractable->InteractableData.InteractionDuration, 0.1f)) // Tweak this
				Interact();
			else
				GetWorldTimerManager().SetTimer(TimerHandle_Interaction,
				                                this, &APlayerCharacter::Interact,
				                                TargetInteractable->InteractableData.InteractionDuration,
				                                false);
		}
	}
}

void APlayerCharacter::EndInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("Calling EndInteract override on PlayerCharacter actor"));
	GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);

	if (IsValid(TargetInteractable.GetObject()))
		TargetInteractable->EndInteract();
}

void APlayerCharacter::Interact()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);

	if (IsValid(TargetInteractable.GetObject()))
	{
		if (TargetInteractable->InteractableData.InteractableType == EInteractableType::Instant)
		{
			TargetInteractable->Interact(this);
			UE_LOG(LogTemp, Warning, TEXT("Interacting with Instant Object"));
		}
		else if (TargetInteractable->InteractableData.InteractableType == EInteractableType::Puzzle)
		{
			if (APuzzlePawn* PuzzlePawn = Cast<APuzzlePawn>(InteractionData.CurrentInteractable))
				ControlPuzzle(PuzzlePawn);
		}
	}
}


void APlayerCharacter::ControlPuzzle(APuzzlePawn* PuzzlePawn)
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (PC && PuzzlePawn)
		{
			PC->UnPossess();
			PC->Possess(PuzzlePawn);
		}
	}
}

// Movement
void APlayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MoveAxisValue = Value.Get<FVector2D>();
	if (GetController() && !MoveAxisValue.IsZero())
	{
		AddMovementInput(GetActorForwardVector(), MoveAxisValue.Y);
		AddMovementInput(GetActorRightVector(), MoveAxisValue.X);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();
	if (GetController())
	{
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void APlayerCharacter::Rotate(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();
	if (PlayerMesh)
	{
		// Determine the rotation amount, you might want to multiply these by a rotation speed factor
		float DeltaYaw = LookAxisValue.X;
		float DeltaPitch = LookAxisValue.Y;

		// Get the current rotation
		FRotator CurrentRotation = PlayerMesh->GetRelativeRotation();

		// Modify the rotation based on the input
		CurrentRotation.Yaw += DeltaYaw;
		CurrentRotation.Pitch += DeltaPitch;

		// Set the new rotation
		PlayerMesh->SetRelativeRotation(CurrentRotation);
	}
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind input
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<
		UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Movement
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);

		// Look
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);

		// Rotate
		EnhancedInputComponent->BindAction(RotateAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Rotate);
		
		// Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this,
		                                   &APlayerCharacter::BeginInteract);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this,
		                                   &APlayerCharacter::EndInteract);
	}
}
