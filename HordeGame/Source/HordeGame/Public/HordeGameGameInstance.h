// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MenuSystem/MenuInterface.h"
#include <OnlineSessionInterface.h>
#include "HordeGameGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class HORDEGAME_API UHordeGameGameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()

public:
	UHordeGameGameInstance(const FObjectInitializer &ObjectInitializer);

	virtual void Init() override;
	//Loads Main menu widget 
	UFUNCTION(Exec, BlueprintCallable)
		void LoadMenuWidget();
	//Loads Game Menu widget
	UFUNCTION(Exec, BlueprintCallable)
		void LoadGameMenu();

	//Become the server of the game
	UFUNCTION(Exec)
		void Host(FString ServerName);

	//Join a server of a game
	UFUNCTION(Exec)
		void Join(const  uint32 Index);

	UFUNCTION(Exec)
		void Leave();

	void RefreshServerList();

	UFUNCTION(BlueprintCallable,Category="GameInstance")
	void StartSession();

private:
	TSubclassOf<class UUserWidget> MenuClass;

	TSubclassOf<class UUserWidget> InGameMenuClass;

	UPROPERTY()
		class UMainMenu * Menu;

	UPROPERTY()
		class UInGameMenu * GameMenu;

	//Steam session
	IOnlineSessionPtr SessionInterface;

	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	FString DesiredServerName;

	void CreateSession();

	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnDestroySessionComplete(FName SessionName, bool Success);
	void OnFindSessionComplete(bool Success);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);


	
	
};
