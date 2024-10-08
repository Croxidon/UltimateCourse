// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GreystoneOverlay.generated.h"

/**
 * 
 */
UCLASS()
class ULTIMATECOURSE_API UGreystoneOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetHealthBarPercent(float Percent);
	void SetStaminaBarPercent(float Percent);
	void SetGold(int32 Gold);
	void SetSouls(int32 Souls);
	void SetSessionText(FString CreatedSessionName);
	void SetSubsystemText(FString OnlineSubsystemName);



private:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaProgressBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GoldText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SoulsText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SubsystemNameText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SessionCreationText;

};
