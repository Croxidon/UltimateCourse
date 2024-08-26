// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HealthBarComponent.h"
#include "HUD/HealthBar.h"
#include "Components/ProgressBar.h"

void UHealthBarComponent::SetHealthPercent(float Percent)
{

	if (HPBarWidget == nullptr)
	{
		HPBarWidget = Cast<UHealthBar>(GetUserWidgetObject());
	}

	if (HPBarWidget && HPBarWidget->HealthBar)
	{
		HPBarWidget->HealthBar->SetPercent(Percent);
	}
}
