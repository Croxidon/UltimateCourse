// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GreystoneHUD.generated.h"

class UGreystoneOverlay;
/**
 * 
 */
UCLASS()
class ULTIMATECOURSE_API AGreystoneHUD : public AHUD
{
	GENERATED_BODY()

public:
	FORCEINLINE UGreystoneOverlay* GetGreystoneOverlay() const { return GreystoneOverlay; };

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TSubclassOf<UGreystoneOverlay> GreystoneOverlayClass;

	UPROPERTY()
	UGreystoneOverlay* GreystoneOverlay;
};
