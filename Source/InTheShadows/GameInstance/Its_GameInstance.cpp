// Fill out your copyright notice in the Description page of Project Settings.


#include "Its_GameInstance.h"
#include "Its_SaveGame.h"
#include "Kismet/GameplayStatics.h"


void UIts_GameInstance::Init()
{
	Super::Init();

	if (UGameplayStatics::DoesSaveGameExist(TEXT("SaveSlot"), 0))
	{
		UIts_SaveGame* LoadGameInstance = Cast<UIts_SaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("SaveSlot"), 0));
		if (LoadGameInstance != nullptr)
		{
			PuzzleStates = LoadGameInstance->SavedPuzzleStates;
			PuzzleTransforms = LoadGameInstance->SavedPuzzleTransforms;
		}
		UE_LOG(LogTemp, Warning, TEXT("Save loaded from game instance"));
	}
	else
	{
		InitializeDefaultPuzzleStates(PuzzleStates);
		UE_LOG(LogTemp, Warning, TEXT("Save not found, puzzle states initialized from game instance"));
	}
}

void UIts_GameInstance::InitializeDefaultPuzzleStates(const TMap<FString, bool>& InitialStates)
{
	PuzzleStates = InitialStates;
	UE_LOG(LogTemp, Warning, TEXT("Puzzle States initialized from game instance : InitializeDefaultPuzzleStates"));
}

void UIts_GameInstance::ResetPuzzleStates()
{
	PuzzleStates.Empty();
	UGameplayStatics::DeleteGameInSlot(TEXT("SaveSlot"), 0);
	UE_LOG(LogTemp, Warning, TEXT("Save deleted from game instance"));
}

void UIts_GameInstance::SetPuzzleState(const FText& PuzzleName, bool bIsCompleted, const FTransform& PuzzleTransform)
{
	PuzzleStates.Add(PuzzleName.ToString(), bIsCompleted);
	PuzzleTransforms.Add(PuzzleName.ToString(), FTransform());

	if (UIts_SaveGame* SaveGameInstance = Cast<UIts_SaveGame>(
		UGameplayStatics::CreateSaveGameObject(UIts_SaveGame::StaticClass())))
	{
		SaveGameInstance->SavedPuzzleStates = PuzzleStates;
		SaveGameInstance->SavedPuzzleTransforms = PuzzleTransforms;
		UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("SaveSlot"), 0);
		
		UE_LOG(LogTemp, Warning, TEXT("Puzzle state set with name %s and transform %s from game instance"), *PuzzleName.ToString(),
	   *PuzzleTransform.ToString());
	}
}

bool UIts_GameInstance::GetPuzzleState(const FText& PuzzleName, FTransform& PuzzleTransform) const
{
	const bool* bIsCompleted = PuzzleStates.Find(PuzzleName.ToString());
	const FTransform* SavedTransform = PuzzleTransforms.Find(PuzzleName.ToString());

	if (bIsCompleted && SavedTransform)
	{
		PuzzleTransform = *SavedTransform;
		UE_LOG(LogTemp, Warning, TEXT("Puzzle state found and loaded from game instance"));
		return *bIsCompleted;
	}
	else
		return false;
}
