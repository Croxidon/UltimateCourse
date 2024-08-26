#pragma once


UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(Displayname = "Unequipped"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Handed Weapon")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(Displayname = "Unoccupied"),
	EAS_HitReaction UMETA(Displayname = "HitReaction"),
	EAS_Attacking UMETA(Displayname = "Attacking"),
	EAS_ArmingWeapon UMETA(Displayname = "Arming Weapon"),
	EAS_Dodging UMETA(Displayname = "Dodging"),
	EAS_Dead UMETA(Displayname = "Dead")
};

UENUM(BlueprintType)
enum class EDeathPose : uint8
{
	EDP_ToFront UMETA(Displayname = "ToFront"),
	EDP_ToBack UMETA(Displayname = "ToBack"),
	EDP_ToRight UMETA(Displayname = "ToRight"),
	EDP_ToLeft UMETA(Displayname = "ToLeft")
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_NoState UMETA(Displayname = "NoState"),
	EES_Dead UMETA(Displayname = "Dead"),
	EES_Patrolling UMETA(Displayname = "Patrolling"),
	EES_Chasing UMETA(Displayname = "Chasing"),
	EES_Attacking UMETA(Displayname = "Attacking"),
	EES_Engaged UMETA(Displayname = "Engaged")
};