// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionWidget.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "InTheShadows/Interfaces/InteractionInterface.h"
#include "InTheShadows/Player/PlayerCharacter.h"

void UInteractionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	InteractionProgressBar->PercentDelegate.BindUFunction(this, FName("UpdateInteractionProgress"));
}

void UInteractionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CurrentInteractionTime = 0.0f;
}

void UInteractionWidget::UpdateWidget(const FInteractableData* InteractableData) const
{
	switch (InteractableData->InteractableType)
	{
	case EInteractableType::Instant:
		InteractionProgressBar->SetVisibility(ESlateVisibility::Collapsed);

		break;
	case EInteractableType::Progress:
		InteractionProgressBar->SetVisibility(ESlateVisibility::Visible);
		break;
	case EInteractableType::Puzzle:
		InteractionProgressBar->SetVisibility(ESlateVisibility::Collapsed);
		NameText->SetText(InteractableData->Name);
		break;
	default: ;
	}
	InteractionImage->SetVisibility(ESlateVisibility::Visible);
	ActionText->SetText(InteractableData->Action);
}

float UInteractionWidget::UpdateInteractionProgress()
{
	return 0.f;
}
