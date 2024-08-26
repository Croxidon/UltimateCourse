// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Loot.generated.h"

/**
 * 
 */
UCLASS()
class ULTIMATECOURSE_API ALoot : public AItem
{
	GENERATED_BODY()

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
private:
	UPROPERTY(EditAnywhere, Category = "Loot Properties")
	int32 Gold;

public:
	FORCEINLINE int32 GetGold() const { return Gold; };
};
