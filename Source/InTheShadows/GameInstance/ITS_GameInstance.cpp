// Fill out your copyright notice in the Description page of Project Settings.


#include "ITS_GameInstance.h"

void UITS_GameInstance::SetPuzzleState(const FText& PuzzleName, bool bIsCompleted)
{
	PuzzleStates.Add(PuzzleName.ToString(), bIsCompleted);
	UE_LOG(LogTemp, Warning, TEXT("SetPuzzleState: %d, with name %s"), bIsCompleted, *PuzzleName.ToString());
}

bool UITS_GameInstance::GetPuzzleState(const FText& PuzzleName) const
{
	if (const bool* bIsCompleted = PuzzleStates.Find(PuzzleName.ToString()))
	{
		return *bIsCompleted;
	}
	return false;
}
