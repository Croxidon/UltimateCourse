// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/GreystoneOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UGreystoneOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

void UGreystoneOverlay::SetStaminaBarPercent(float Percent)
{
	if (StaminaProgressBar)
	{
		StaminaProgressBar->SetPercent(Percent);
	}
}

void UGreystoneOverlay::SetGold(int32 Gold)
{
	if (GoldText)
	{
		GoldText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Gold)));
	}
}

void UGreystoneOverlay::SetSouls(int32 Souls)
{
	if (SoulsText)
	{
		SoulsText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Souls)));
	}
}

void UGreystoneOverlay::SetSessionText(FString CreatedSessionName)
{
	if (SessionCreationText)
	{
		SessionCreationText->SetText(FText::FromString(CreatedSessionName));
	}
}

void UGreystoneOverlay::SetSubsystemText(FString OnlineSubsystemName)
{
	if (SubsystemNameText)
	{
		SubsystemNameText->SetText(FText::FromString(OnlineSubsystemName));
	}
}
