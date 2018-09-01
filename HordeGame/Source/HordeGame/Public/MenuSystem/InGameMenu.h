// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameMenu.generated.h"

/**
*
*/
UCLASS()
class HORDEGAME_API UInGameMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup();

	void SetMenuInterface(class IMenuInterface * MenuInterface);
protected:
	virtual bool Initialize() override;



	virtual void OnLevelRemovedFromWorld(ULevel * InLevel, UWorld * InWorld) override;

private:



	UPROPERTY(meta = (BindWidget))
		class UButton* Cancel;

	UPROPERTY(meta = (BindWidget))
		class UButton* Quit;

	UFUNCTION()
		void OnCancelClicked();

	UFUNCTION()
		void OnQuitClicked();

	class IMenuInterface * MenuInterface;

};
