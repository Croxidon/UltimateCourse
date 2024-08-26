// Fill out your copyright notice in the Description page of Project Settings.


#include "Destructible/DestructibleActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Items/Loot.h"

// Sets default values
ADestructibleActor::ADestructibleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("Geometry Collection"));
	SetRootComponent(GeometryCollection);
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetNotifyBreaks(true);

}

// Called when the game starts or when spawned
void ADestructibleActor::BeginPlay()
{
	Super::BeginPlay();
	
	GeometryCollection->OnChaosBreakEvent.AddDynamic(this, &ADestructibleActor::ChaosBreakEvent);
}

void ADestructibleActor::ChaosBreakEvent(const FChaosBreakEvent& BreakEvent)
{
		
	if (!bIsBroken)
	{
		SetLifeSpan(3);
		bIsBroken = true;
		UWorld* World = GetWorld();
		if (World && !TreasureClasses.IsEmpty())
		{
			FVector Location = GetActorLocation();
			Location.Z += 75.f;

			const int32 Selection = FMath::RandRange(0, TreasureClasses.Num() - 1);
			World->SpawnActor<ALoot>(TreasureClasses[Selection], Location, GetActorRotation());



		}
	}
	
}

// Called every frame
void ADestructibleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADestructibleActor::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{

}

