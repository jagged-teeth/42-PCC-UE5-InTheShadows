// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "Components/TimelineComponent.h"
#include "InTheShadows/Interfaces/InteractionInterface.h"
#include "PuzzlePawn.generated.h"

struct FInputActionValue;
class APlayerCharacter;
class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class UStaticMeshComponent;
class UTimelineComponent;
class APlayerHUD;

UCLASS(Blueprintable)
class INTHESHADOWS_API APuzzlePawn : public APawn, public IInteractionInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APuzzlePawn();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Public Misc Functions
	void SetPlayerRef(APlayerCharacter* Player) { PlayerRef = Player; };
	virtual void EndFocus() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Components
	UPROPERTY(EditInstanceOnly, Category = "Puzzle | Interactable")
	FInteractableData InstanceInteractableData;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere)
	UTimelineComponent* Timeline;

	UPROPERTY(EditAnywhere)
	UTimelineComponent* FloatingAnimation;

	// Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Puzzle | Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Puzzle | Input")
	UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Puzzle | Input")
	UInputAction* RollAction;

	// Timeline Events
	FTimeline FloatingTimeline;

	UPROPERTY(EditAnywhere, Category = "Puzzle | Floating Animation")
	UCurveFloat* FloatingCurve;

	UFUNCTION()
	void HandleFloatingTimelineProgress(float Value);

	// Puzzle State
	UPROPERTY(EditAnywhere, Category = "Puzzle | State")
	float RotationTolerance;

	UPROPERTY(EditAnywhere, Category = "Puzzle | State", ToolTip = "Positive values only")
	FRotator TargetRotation;

	UPROPERTY(EditAnywhere, Category = "Puzzle | State")
	bool bIsPuzzleSolved = false;

	// Interface Override
	virtual void BeginFocus() override;
	virtual void BeginInteract() override;
	virtual void EndInteract() override;
	virtual void Interact(APlayerCharacter* PC) override;

	// Input to Possess
	void StartPossessing();
	void OnLongPressComplete();

	// Movement Input
	void Look(const FInputActionValue& Value);
	void Roll(const FInputActionValue& Value);
	void StopRoll() { bIsRollActive = false; }

	// Solve check
	bool IsRotationValid(const FRotator& TargetRotation, float Tolerance) const;
	void SetPuzzleSolved(bool Solved);
	FTransform PuzzleTransform;

private:
	UPROPERTY(EditAnywhere)
	UCameraComponent* PuzzleCamera;

	UPROPERTY()
	APlayerCharacter* PlayerRef;

	FTimerHandle InteractTimerHandle;

	bool bIsFloating = false;
	bool bIsRollActive;

	float StartLocation;
	float LongPressDuration = 1.0f;
};
