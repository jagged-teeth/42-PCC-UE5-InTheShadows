// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "InTheShadows/Interfaces/InteractionInterface.h"
#include "PlayerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UStaticMeshComponent;
class UCameraComponent;
class USpringArmComponent;
class APuzzlePawn;
class APlayerHUD;

USTRUCT()
struct FInteractionData
{
	GENERATED_BODY()

	FInteractionData() : CurrentInteractable(nullptr), LastInteractionCheckTime(0.0f)
	{
	};

	UPROPERTY()
	AActor* CurrentInteractable;

	UPROPERTY()
	float LastInteractionCheckTime;
};

UCLASS()
class INTHESHADOWS_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE bool IsInteracting() const { return GetWorldTimerManager().IsTimerActive(TimerHandle_Interaction); };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	APlayerHUD* HUD;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Input")
	UInputMappingContext* BaseMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Input")
	UInputAction* InteractAction;

	UPROPERTY(VisibleAnywhere, Category = "Character | Interaction")
	TScriptInterface<IInteractionInterface> TargetInteractable;

	float InteractionCheckFrequency;
	float InteractionCheckDistance;
	FTimerHandle TimerHandle_Interaction;
	FInteractionData InteractionData;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void PerformInteractionCheck();
	void FoundInteractable(AActor* NewInteractable);
	void LostInteractable();
	void BeginInteract();
	void EndInteract();
	void Interact();

	void ControlPuzzle();
	void StopControllingPuzzle();

private:
	// Mesh being displayed
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* PlayerMesh;

	// Camera boom to position the camera above the character
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	// Camera to view the scene
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	APuzzlePawn* ControlledPuzzle = nullptr;

};
