// Fill out your copyright notice in the Description page of Project Settings.


#include "InTheShadowsGameMode.h"
#include "GameFramework/DefaultPawn.h"
#include "InTheShadows/Pawns/PuzzlePawn.h"
#include "InTheShadows/Player/PlayerCharacter.h"
#include "IntheShadows/HUD/PlayerHUD.h"
#include "InTheShadows/GameInstance/Its_GameInstance.h"
#include "Kismet/GameplayStatics.h"

AInTheShadowsGameMode::AInTheShadowsGameMode()
{
	// Set default pawn class to our Blueprinted character, HUD, etc.
	if (DefaultPawnClass == ADefaultPawn::StaticClass() || !DefaultPawnClass)
			DefaultPawnClass = APlayerCharacter::StaticClass();

	if (HUDClass == AHUD::StaticClass() || !HUDClass)
		HUDClass = APlayerHUD::StaticClass();
}

// void AInTheShadowsGameMode::InitializeDefaultPuzzleStates()
// {
// 	TArray<AActor*> FoundPuzzles;
// 	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APuzzlePawn::StaticClass(), FoundPuzzles);
//
// 	if (UIts_GameInstance* GI = Cast<UIts_GameInstance>(GetGameInstance()))
// 	{
// 		for (AActor* Actor : FoundPuzzles)
// 		{
// 			if (APuzzlePawn* Puzzle = Cast<APuzzlePawn>(Actor))
// 			{
// 				GI->SetPuzzleState(Puzzle->InteractableData.Name, false);
// 				// GI->SetPuzzleState(Puzzle->InteractableData.Name, false, Puzzle->GetActorTransform());
// 				UE_LOG(LogTemp, Warning, TEXT("INIT DEFAULT Puzzle state loaded from game mode"));
// 			}
// 		}
// 	}
// }

void AInTheShadowsGameMode::BeginPlay()
{
	Super::BeginPlay();
	// InitializeDefaultPuzzleStates();
}
