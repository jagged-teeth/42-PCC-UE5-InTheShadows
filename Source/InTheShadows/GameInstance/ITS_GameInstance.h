// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ITS_GameInstance.generated.h"

UCLASS()
class INTHESHADOWS_API UITS_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	
	void SetPuzzleState(const FText& PuzzleName, bool bIsCompleted);
	bool GetPuzzleState(const FText& PuzzleName) const;
	void InitializeDefaultPuzzleStates(const TMap<FString, bool>& InitialStates);
private:
	TMap<FString, bool> PuzzleStates;
};
