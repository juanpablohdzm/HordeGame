// Fill out your copyright notice in the Description page of Project Settings.

#include "HordeGameGameInstance.h"
#include <Engine/Engine.h>
#include <UObject/ConstructorHelpers.h>
#include "MenuSystem/MainMenu.h"
#include "MenuSystem/InGameMenu.h"
#include <Blueprint/UserWidget.h>
#include <OnlineSubsystem.h>
#include <OnlineSessionSettings.h>

#include <string>

const static FName SESSION_NAME = TEXT("GameSession");
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");

UHordeGameGameInstance::UHordeGameGameInstance(const FObjectInitializer &ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UMainMenu> MenuBPClass(TEXT("/Game/UI/BP/WBP_MainMenu")); 
	if (!ensure(MenuBPClass.Class != NULL)) { UE_LOG(LogTemp, Warning, TEXT("Menu BP Class missing")); return; }
	MenuClass = MenuBPClass.Class;

	static ConstructorHelpers::FClassFinder<UInGameMenu> InGameMenuBPClass(TEXT("/Game/UI/BP/WBP_InGameMenu"));
	if (!ensure(InGameMenuBPClass.Class != NULL)) { UE_LOG(LogTemp, Warning, TEXT("In Game Menu BP Class missing")); return; }
	InGameMenuClass = InGameMenuBPClass.Class;


}


void UHordeGameGameInstance::Init()
{
	//All of this is done for the steam session, remember dependencies in .Build.cs and libraries and DefaultEngine.ini
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Subsytem found %s"), *Subsystem->GetSubsystemName().ToString());
		SessionInterface = Subsystem->GetSessionInterface(); //Shared pointer
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UHordeGameGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UHordeGameGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UHordeGameGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UHordeGameGameInstance::OnJoinSessionComplete);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No subsytem found"));
	}
}


void UHordeGameGameInstance::LoadMenuWidget()
{

	if (!ensure(MenuClass != nullptr)) { UE_LOG(LogTemp, Warning, TEXT("Menu Class missing")); return; }
	Menu = CreateWidget<UMainMenu>(this, MenuClass);

	if (!ensure(Menu != nullptr)) { UE_LOG(LogTemp, Warning, TEXT("Menu  missing")); return; }
	Menu->Setup();

	Menu->SetMenuInterface(this);
}

void UHordeGameGameInstance::LoadGameMenu()
{

	if (!ensure(InGameMenuClass != nullptr)) return;
	GameMenu = CreateWidget<UInGameMenu>(this, InGameMenuClass);

	if (!ensure(GameMenu != nullptr)) return;
	GameMenu->Setup();

	GameMenu->SetMenuInterface(this);


}

void UHordeGameGameInstance::CreateSession()
{
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings SessionSettings;
		if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
		{
			SessionSettings.bIsLANMatch = true;
		}
		else
		{
			//This is over the network
			SessionSettings.bIsLANMatch = false;
		}
		SessionSettings.NumPublicConnections = 3;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bUsesPresence = true;
		SessionSettings.Set(SERVER_NAME_SETTINGS_KEY, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
		//At the end of creating the session due to the delegate at init OnCreateSessionComplete will be called
	}
}

void UHordeGameGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if (!Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not create session"));
	}
	UWorld * World = GetWorld();
	if (!ensure(World != nullptr)) return;

	//If you are in a map the server and the world travels with all the clients to the specific url
	World->ServerTravel("/Game/Maps/Blockout_P?listen");
}

void UHordeGameGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if (Success)
	{
		CreateSession();
	}

}

void UHordeGameGameInstance::Host(FString ServerName)
{
	//At this point MainMenu will call OnLevelRemovedFromWorld
	DesiredServerName = ServerName;
	if (SessionInterface.IsValid())
	{
		FNamedOnlineSession * ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
		if (ExistingSession != nullptr)
		{
			SessionInterface->DestroySession(SESSION_NAME);
		}
		else
		{
			CreateSession();
		}
	}


}


void UHordeGameGameInstance::OnFindSessionComplete(bool Success)
{
	if (Success && SessionSearch.IsValid() && Menu != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("FindSessionComplete"));
		TArray<FServerData> ServerNames;
		for (FOnlineSessionSearchResult &result : SessionSearch->SearchResults)
		{
			FServerData Data;
			Data.Name = result.GetSessionIdStr();
			Data.MaxPlayers = result.Session.SessionSettings.NumPublicConnections;
			Data.CurrentPlayers = Data.MaxPlayers - result.Session.NumOpenPublicConnections;
			Data.HostUserName = result.Session.OwningUserName;
			FString ServerName;
			if (result.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, ServerName))
			{
				Data.Name = ServerName;
			}
			else
			{
				Data.Name = "Could not find name.";
			}
			ServerNames.Add(Data);
		}

		Menu->SetServerList(ServerNames);
	}
}



//RefreshServerList or Join button on main menu
void UHordeGameGameInstance::RefreshServerList()
{
	//At joingame button to choose a session this happends 

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid())
	{
		//SessionSearch->bIsLanQuery = true;
		SessionSearch->MaxSearchResults = 100;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}

void UHordeGameGameInstance::Join(const  uint32 Index)
{
	if (!SessionInterface.IsValid()) return;
	if (!SessionSearch.IsValid()) return;
	SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);

}

void UHordeGameGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!SessionInterface.IsValid())return;
	FString Address;
	if (!SessionInterface->GetResolvedConnectString(SessionName, Address))
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not get connect string"));
		return;
	}
	//Client travel only move one controller at a time 

	APlayerController * PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}

void UHordeGameGameInstance::StartSession()
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->StartSession(SESSION_NAME);
	}
}


void UHordeGameGameInstance::Leave()
{
	APlayerController * PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	if (PlayerController->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Server"));
		UWorld * World = GetWorld();
		if (!ensure(World != nullptr)) return;

		//If you are in a map the server and the world travels with all the clients to the specific url
		World->ServerTravel("/Game/Maps/MainMenu?listen");

		if (SessionInterface.IsValid())
		{
			SessionInterface->DestroySession(SESSION_NAME);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Client"));
		APlayerController * PlayerController = GetFirstLocalPlayerController();
		if (!ensure(PlayerController != nullptr)) return;

		PlayerController->ClientTravel("/Game/Maps/MainMenu", ETravelType::TRAVEL_Absolute);
	}
}




