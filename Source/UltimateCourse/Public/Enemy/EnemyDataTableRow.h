#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EnemyDataTableRow.generated.h"

UENUM(BlueprintType)
enum class EEnemyDeathDirection : uint8
{
    Front UMETA(DisplayName = "ToFront"),
    Right UMETA(DisplayName = "ToRight"),
    Left UMETA(DisplayName = "ToLeft"),
    Back UMETA(DisplayName = "ToBack")
};


USTRUCT(BlueprintType)
struct FEnemyDataTableRow : public FTableRowBase
{
    GENERATED_BODY()

public:
    // Soft reference to the death pose animation
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enemy Data")
    TMap<EEnemyDeathDirection, TObjectPtr<UAnimSequence>> DeathPoseAnimations;
};
    