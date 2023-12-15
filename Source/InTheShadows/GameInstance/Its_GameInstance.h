// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "InTheShadows/Pawns/PuzzlePawn.h"
#include "Its_GameInstance.generated.h"

UCLASS()
class INTHESHADOWS_API UIts_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	void SetPuzzleState(const FText& PuzzleName, const FPuzzleState& State, const FTransform& PuzzleTransform);
	// void InitializeDefaultPuzzleStates(const TMap<FString, bool>& InitialStates);

	UFUNCTION(BlueprintCallable)
	FPuzzleState GetPuzzleState(const FText& PuzzleName, FTransform& PuzzleTransform) const;

	UFUNCTION(BlueprintCallable)
	void ResetPuzzleStates();

	// Use this in the menu widget
	UFUNCTION(BlueprintCallable)
	bool GetIsSaveDeleted() const { return bIsSaveDeleted; };

private:
	TMap<FString, FPuzzleState> PuzzleStates;
	TMap<FString, FTransform> PuzzleTransforms;
	bool bIsSaveDeleted = false;
};
