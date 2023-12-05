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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Components
	UPROPERTY(EditInstanceOnly, Category = "Interactable")
	FInteractableData InstanceInteractableData;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere)
	UTimelineComponent* Timeline;

	UPROPERTY(EditAnywhere)
	UTimelineComponent* FloatingAnimation;

	// Timeline Events
	FTimeline FloatingTimeline;

	UPROPERTY(EditAnywhere, Category = "Floating Animation")
	UCurveFloat* FloatingCurve;

	UFUNCTION()
	void HandleFloatingTimelineProgress(float Value);

	// Interface Override
	virtual void BeginFocus() override;
	virtual void EndFocus() override;
	virtual void BeginInteract() override;
	virtual void EndInteract() override;
	virtual void Interact(APlayerCharacter* PC) override;
	void StartInteract();
	void OnLongPressComplete();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Puzzle | Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Puzzle | Input")
	UInputAction* InteractAction;

	void Look(const FInputActionValue& Value);

private:
	UPROPERTY(EditAnywhere)
	UCameraComponent* PuzzleCamera;

	UPROPERTY()
	APlayerCharacter *PlayerRef;

	UPROPERTY(VisibleInstanceOnly)
	APlayerHUD* PlayerHUD = nullptr;
	
	bool bIsFloating = false;
	float StartLocation;

	FTimerHandle InteractTimerHandle;
	float LongPressDuration = 1.0f;
};
