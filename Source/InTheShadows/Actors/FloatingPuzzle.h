// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "InTheShadows/Interfaces/InteractionInterface.h"
#include "FloatingPuzzle.generated.h"

struct FInputActionValue;
class USphereComponent;
class UStaticMeshComponent;
class URotatingMovementComponent;
class UTimelineComponent;

UCLASS(Blueprintable)
class INTHESHADOWS_API AFloatingPuzzle : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFloatingPuzzle();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Set puzzle control active
	void SetPuzzleControlActive(bool bIsActive);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Components
	UPROPERTY(VisibleInstanceOnly, Category = "Interactable")
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
	UFUNCTION(BlueprintCallable)
	virtual void BeginFocus() override;
	UFUNCTION(BlueprintCallable)
	virtual void EndFocus() override;
	UFUNCTION(BlueprintCallable)
	virtual void BeginInteract() override;
	UFUNCTION(BlueprintCallable)
	virtual void EndInteract() override;
	UFUNCTION(BlueprintCallable)
	virtual void Interact(APlayerCharacter* PC) override;

private:
	bool bIsFloating = false;
	float StartLocation;
	bool bIsControlledByPlayer = false;

	// void RotatePuzzle(FInputActionValue &Value);
};
