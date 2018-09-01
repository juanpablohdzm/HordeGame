// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerRow.generated.h"

/**
*
*/
UCLASS()
class HORDEGAME_API UServerRow : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(meta = (BindWidget))
		class UTextBlock * ServerName;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock * HostUser;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock * ConnectionFraction;

	void SetUp(class UMainMenu * Parent, uint32 Index);

	UPROPERTY(BlueprintReadOnly)
		bool Selected = false;

private:
	UPROPERTY(meta = (BindWidget))
		class UButton * RowButton;

	UFUNCTION()
		void OnClicked();

	UPROPERTY()
		class UMainMenu * Parent;

	uint32 Index;
};
