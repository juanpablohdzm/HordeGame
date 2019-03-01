// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameMenu.h"

#include "MenuSystem/MenuInterface.h"
#include <Components/Button.h>



void UInGameMenu::SetMenuInterface(IMenuInterface * MenuInterface)
{
	this->MenuInterface = MenuInterface;
}

bool UInGameMenu::Initialize()
{
	bool result = Super::Initialize();
	if (!result) return false;

	if (!ensure(Quit != nullptr))return false;
	Quit->OnClicked.AddDynamic(this, &UInGameMenu::OnQuitClicked);

	return true;
}

void UInGameMenu::Setup()
{
	this->AddToViewport();

	APlayerController * PlayerController = GetWorld()->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus(this->TakeWidget()); //->TakeWidget() transforms Widget to SWidget
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	PlayerController->SetInputMode(InputModeData);
	PlayerController->bShowMouseCursor = true;
}

void UInGameMenu::OnLevelRemovedFromWorld(ULevel * InLevel, UWorld * InWorld)
{
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);

	this->RemoveFromViewport();
	APlayerController * PlayerController = GetWorld()->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	FInputModeGameOnly InputModeData;
	PlayerController->SetInputMode(InputModeData);
	PlayerController->bShowMouseCursor = false;
}

void UInGameMenu::OnQuitClicked()
{
	
	if (MenuInterface != nullptr)
	{
		MenuInterface->Leave();
	}
}
