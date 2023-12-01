// Fill out your copyright notice in the Description page of Project Settings.


#include "InTheShadowsGameMode.h"
#include "GameFramework/DefaultPawn.h"
#include "InTheShadows/Player/PlayerCharacter.h"
#include "IntheShadows/HUD/PlayerHUD.h"

AInTheShadowsGameMode::AInTheShadowsGameMode()
{
	// Set default pawn class to our Blueprinted character, HUD, etc.
	if (DefaultPawnClass == ADefaultPawn::StaticClass() || !DefaultPawnClass)
			DefaultPawnClass = APlayerCharacter::StaticClass();

	if (HUDClass == AHUD::StaticClass() || !HUDClass)
		HUDClass = APlayerHUD::StaticClass();
}
