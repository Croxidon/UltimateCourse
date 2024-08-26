// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterStates.h"
#include "Enemy.generated.h"

class UHealthBarComponent;
class UPawnSensingComponent;

UCLASS()
class ULTIMATECOURSE_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	// <AActor>
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;
	// </AActor>

	//<IHitInterface>
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	//</IHitInterface>


protected:
	// <AActor
	virtual void BeginPlay() override;
	// </AActor>

	// <ABaseCharacter>
	void Die_Implementation(const FName& SectionName) override;
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual int32 PlayDeathMontage() override;
	virtual void AttackEnd() override;
	// </ABaseCharacter>



	UPROPERTY(BlueprintReadOnly, Visibleanywhere)
	EEnemyState EnemyState{ EEnemyState::EES_Patrolling };

	

private:
	// AI Behaviour
	void InitializeEnemy();
	void CheckCombatTarget();
	void CheckPatrolTarget();
	void PatrolTimerFinished();
	void HealthBarVisibilitiy(bool OnOff);
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();
	bool IsChasing();
	bool IsAttacking();
	bool IsEngaged();
	bool IsDead();
	void ClearPatrolTimer();
	void UpdateRotationToFaceTarget();
	void StartAttackTimer();
	void ClearAttackTimer();
	bool InTargetRange(AActor* Target, double radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();
	void SpawnDefaultWeapon();
	void SpawnSoul();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn); // Callback for Pawn Seen on UPawnSensingComponent


	UPROPERTY()
	class AAIController* EnemyController;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY()
	double PatrolRadius{ 300.f };

	UPROPERTY(EditAnywhere, Category = "Combat")
	float PatrollingSpeed{ 200.f };

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChasingSpeed{ 300.f };

	UPROPERTY(EditAnywhere, Category = "Combat")
	double CombatRadius{ 750.f };

	UPROPERTY(EditAnywhere, Category = "Combat")
	double AttackRadius{ 200.f };

	UPROPERTY(EditAnywhere, Category = "Combat")
	double MoveToTargetAcceptanceRadius{ 50.f };

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMin{ 0.5f };
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMax{ 1.f };

	

	FTimerHandle PatrolTimer;
	FTimerHandle AttackTimer;

	/*
	Components
	*/
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HPBarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class ASoul> SoulClass;

	const FVector FrontVector = FVector(1.0f, 0.0f, 0.0f);
	const FVector RightVector = FVector(0.0f, 1.0f, 0.0f);
	const FVector BackVector = FVector(-1.0f, 0.0f, 0.0f);
	const FVector LeftVector = FVector(0.0f, -1.0f, 0.0f);

	
};
