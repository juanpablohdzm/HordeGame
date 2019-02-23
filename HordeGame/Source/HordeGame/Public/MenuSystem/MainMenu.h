// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuInterface.h"
#include "MainMenu.generated.h"

USTRUCT()
struct FServerData
{
	GENERATED_BODY()

	FString Name;
	uint16 CurrentPlayers;
	uint16 MaxPlayers;
	FString HostUserName;
};
/**
*
*/
UCLASS()
class HORDEGAME_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	UMainMenu(const FObjectInitializer &ObjectInitializer);

	void SetMenuInterface(IMenuInterface * MenuInterface);

	void Setup();

	void SetServerList(const TArray<FServerData> &ServerNames);

	void SelectIndex(uint32 Option);

protected:
	virtual bool Initialize() override;

	virtual void OnLevelRemovedFromWorld(ULevel * InLevel, UWorld * InWorld) override;

	//Names have to be the same as in blueprint when you use BindWidget
	UPROPERTY(meta = (BindWidget),VisibleAnywhere,BlueprintReadWrite)
		class UButton * Host;

	UPROPERTY(meta = (BindWidget))
		class UButton * JoinGame;


	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadWrite)
		class UWidgetSwitcher* SwitcherMenu;

	UPROPERTY(meta = (BindWidget))
		class UWidget* JoinMenu;
	

	UPROPERTY(meta = (BindWidget))
		class UButton * Join;
	
	UPROPERTY(meta = (BindWidget))
		class UEditableText * HostName;

	UPROPERTY(meta = (BindWidget))
		class UScrollBox * ServerList;

	UFUNCTION()
		void OnHostClick();

	UFUNCTION()
		void OnJoinGameClick();

	UFUNCTION()
		void OpenJoinMenu();

	

	IMenuInterface * MenuInterface;

	TSubclassOf<class UUserWidget> ServerRowClass;

	TOptional<uint32> SelectedIndex;

	void UpdateChildren();

	
};
