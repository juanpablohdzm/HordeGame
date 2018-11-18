// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenu.h"

#include <Components/Button.h>
#include <Components/WidgetSwitcher.h>
#include <Components/EditableText.h>
#include <Components/ScrollBox.h>
#include <Components/TextBlock.h>
#include "MenuSystem/ServerRow.h"
#include <UObject/ConstructorHelpers.h>


UMainMenu::UMainMenu(const FObjectInitializer &ObjectInitializer) :Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UServerRow> ServerRowBPClass(TEXT("/Game/Blueprints/UI/WBP_ServerRow"));
	if (!ensure(ServerRowBPClass.Class != NULL)) return;
	ServerRowClass = ServerRowBPClass.Class;
}

bool UMainMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	if (!ensure(Host != nullptr)) return false;
	Host->OnClicked.AddDynamic(this, &UMainMenu::OnHostClick);

	if (!ensure(HostMenuButton != nullptr)) return false;
	HostMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OpenHostMenu);

	if (!ensure(Join != nullptr)) return false;
	Join->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

	if (!ensure(Options != nullptr)) return false;
	Options->OnClicked.AddDynamic(this, &UMainMenu::OpenOptionsMenu);

	if (!ensure(Back != nullptr)) return false;
	Back->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);
	
	if (!ensure(BackHost != nullptr)) return false;
	BackHost->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

	if (!ensure(BackControls != nullptr)) return false;
	BackControls->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);


	if (!ensure(JoinGame != nullptr)) return false;
	JoinGame->OnClicked.AddDynamic(this, &UMainMenu::OnJoinGameClick);

	

	return true;
}

void UMainMenu::OnLevelRemovedFromWorld(ULevel * InLevel, UWorld * InWorld)
{
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);

	this->RemoveFromViewport();
	APlayerController * PlayerController = GetWorld()->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	FInputModeGameOnly InputModeData;
	PlayerController->SetInputMode(InputModeData);
	PlayerController->bShowMouseCursor = false;
}

void UMainMenu::UpdateChildren()
{
	for (int i = 0; i < ServerList->GetChildrenCount(); i++)
	{
		UServerRow * Row = Cast<UServerRow>(ServerList->GetChildAt(i));
		if (Row != nullptr)
		{
			Row->Selected = (SelectedIndex.IsSet() && SelectedIndex.GetValue() == i);
		}
	}
}

void UMainMenu::SelectIndex(uint32 Option)
{
	SelectedIndex = Option;
	UpdateChildren();
}

void UMainMenu::SetServerList(const TArray<FServerData> &ServerNames)
{

	UWorld * World = this->GetWorld();
	if (World == nullptr) return;

	ServerList->ClearChildren();

	for (int i = 0; i < ServerNames.Num();i++)
	{
		UServerRow * ServerRow = CreateWidget < UServerRow>(World, ServerRowClass);
		if (ServerRow == nullptr) return;

		ServerRow->ServerName->SetText(FText::FromString(ServerNames[i].Name));
		ServerRow->HostUser->SetText(FText::FromString(ServerNames[i].HostUserName));
		ServerRow->ConnectionFraction->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), ServerNames[i].CurrentPlayers, ServerNames[i].MaxPlayers)));
		ServerRow->SetUp(this, i);
		ServerList->AddChild(ServerRow);
	}
}

void UMainMenu::SetMenuInterface(IMenuInterface * MenuInterface)
{
	this->MenuInterface = MenuInterface;
}

void UMainMenu::Setup()
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

void UMainMenu::OnHostClick()
{
	if (MenuInterface != nullptr)
	{
		FString name = HostName->GetText().ToString();
		MenuInterface->Host(name);
	}
}

void UMainMenu::OnJoinGameClick()
{
	if (MenuInterface != nullptr)
	{
		if (SelectedIndex.IsSet())
			MenuInterface->Join(SelectedIndex.GetValue());
	}
}

void UMainMenu::OpenJoinMenu()
{
	if (!ensure(SwitcherMenu != nullptr)) return;
	if (!ensure(JoinMenu != nullptr)) return;
	SwitcherMenu->SetActiveWidget(JoinMenu);

	//Steam service 
	MenuInterface->RefreshServerList();
}

void UMainMenu::OpenMainMenu()
{
	if (!ensure(SwitcherMenu != nullptr)) return;
	if (!ensure(MainMenu != nullptr)) return;
	SwitcherMenu->SetActiveWidget(MainMenu);
}


void UMainMenu::OpenHostMenu()
{
	if (!ensure(SwitcherMenu != nullptr)) return;
	if (!ensure(JoinMenu != nullptr)) return;
	SwitcherMenu->SetActiveWidget(HostMenu);
}

void UMainMenu::OpenOptionsMenu()
{
	if (!ensure(SwitcherMenu != nullptr)) return;
	if (!ensure(ControlsMenu != nullptr)) return;
	SwitcherMenu->SetActiveWidget(ControlsMenu);
}

