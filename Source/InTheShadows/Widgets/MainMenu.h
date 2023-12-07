// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"

class APlayerCharacter;

UCLASS()
class INTHESHADOWS_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY()
	APlayerCharacter* GameCharacter;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
};
