// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/GreystoneHUD.h"
#include "HUD/GreystoneOverlay.h"


void AGreystoneHUD::BeginPlay()
{
	Super::BeginPlay();

	if (UWorld* World = GetWorld())
	{
		APlayerController* Controller = World->GetFirstPlayerController();

		if (Controller && GreystoneOverlayClass)
		{
			GreystoneOverlay = CreateWidget<UGreystoneOverlay>(Controller, GreystoneOverlayClass);
			GreystoneOverlay->AddToViewport();
		}
	}
}
