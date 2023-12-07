// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"
#include "InTheShadows/Player/PlayerCharacter.h"

void UMainMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	// GameCharacter = Cast<APlayerCharacter>(GetOwningPlayerPawn());
}