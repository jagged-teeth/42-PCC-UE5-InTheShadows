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
class USphereComponent;
class UStaticMeshComponent;
class URotatingMovementComponent;
class UTimelineComponent;

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
	USphereComponent* SphereCollider;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	URotatingMovementComponent* RotatingMovement;

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
	virtual void Interact(APlayerCharacter* PC) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Input")
	UInputAction* LookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Input")
	UInputAction* RotateAction;
	
	void Look(const FInputActionValue& Value);
	void Rotate(const FInputActionValue& Value);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* PuzzleCamera;

	bool bIsFloating = false;
	float StartLocation;

	UPROPERTY()
	float RotationSpeed = 100.f;
};
