// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include "InTheShadows/Widgets/MainMenu.h"
#include "InTheShadows/Widgets/InteractionWidget.h"


APlayerHUD::APlayerHUD()
{
}

void APlayerHUD::BeginPlay()
{
	Super::BeginPlay();

	if (MainMenuClass)
	{
		MainMenuWidget = CreateWidget<UMainMenu>(GetWorld(), MainMenuClass);
		MainMenuWidget->AddToViewport(1);
		MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed); // Collapsed > Hidden performance-wise
	}

	if (InteractionWidgetClass)
	{
		InteractionWidget = CreateWidget<UInteractionWidget>(GetWorld(), InteractionWidgetClass);
		InteractionWidget->AddToViewport(0);
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed); // Collapsed > Hidden performance-wise
	}
}


void APlayerHUD::DisplayMenu()
{
	if (MainMenuWidget)
	{
		bIsMenuDisplayed = true;
		if (APlayerController* PC = GetOwningPlayerController())
		{
			PC->bShowMouseCursor = true;
			PC->SetInputMode(FInputModeGameAndUI());
		}
		
		MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void APlayerHUD::HideMenu()
{
	if (MainMenuWidget)
	{
		bIsMenuDisplayed = false;
		if (APlayerController* PC = GetOwningPlayerController())
		{
			PC->bShowMouseCursor = false;
			PC->SetInputMode(FInputModeGameOnly());
		}
		
		MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void APlayerHUD::ShowInteractionWidget() const
{
	if (InteractionWidget)
		InteractionWidget->SetVisibility(ESlateVisibility::Visible);
}

void APlayerHUD::HideInteractionWidget() const
{
	if (InteractionWidget)
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void APlayerHUD::UpdateInteractionWidget(const FInteractableData* InteractableData) const
{
	if (InteractionWidget)
	{
		if (InteractionWidget->GetVisibility() == ESlateVisibility::Collapsed)
			InteractionWidget->SetVisibility(ESlateVisibility::Visible);

		InteractionWidget->UpdateWidget(InteractableData);
	}
}
