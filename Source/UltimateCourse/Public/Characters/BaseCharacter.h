// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "CharacterStates.h"
#include "BaseCharacter.generated.h"


class AWeapon;
class UAttributeComponent;
class UAnimMontage;



UCLASS()
class ULTIMATECOURSE_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	UFUNCTION(BlueprintCallable)


virtual void Tick(float DeltaTime) override;

protected:
	
	virtual void BeginPlay() override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void Attack();

	UFUNCTION(BlueprintNativeEvent)
	void Die(const FName& SectionName);


	void DirectionalHitReact(const FVector& ImpactPoint, bool IsAlive);
	virtual void HandleDamage(float DamageAmount);
	void PlayGetHitSound(const FVector& ImpactPoint);
	void SpawnGetHitParticles(const FVector& ImpactPoint);
	virtual bool CanAttack();
	bool IsAlive();
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();
	UFUNCTION(BlueprintCallable)
	virtual	void DodgeEnd();


	/* Montages */
	void PlayHitReactMontage(const FName& SectionName);
	virtual int32 PlayDeathMontage();
	virtual int32 PlayAttackMontage();
	virtual void PlayDodgeMontage();
	void StopAttackMontage();
	
	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();
	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();
	
	void SetCapsuleCollision(ECollisionEnabled::Type OnOff);
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	UFUNCTION(BlueprintCallable)
	virtual void HitReactEnd();

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	AWeapon* ArmedWeapon;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double WarpToTargetDistance = 20.f;

	/* Components */
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes; 

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose;

	TMap<FName, EDeathPose> SectionToDeathPoseMap = {
		{FName("Right"), EDeathPose::EDP_ToLeft},
		{FName("Left"), EDeathPose::EDP_ToRight},
		{FName("Back"), EDeathPose::EDP_ToFront},
		{FName("Front"), EDeathPose::EDP_ToBack}
	};

private:

	virtual void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);


	UPROPERTY(EditAnywhere, Category = "Combat")
	USoundBase* GetHitSound;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UParticleSystem* GetHitParticles;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float DeathLifeSpan{ 5.f };
	
	/* Animation Montages */

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* AttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* HitReactMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* DeathMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* DodgeMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName> AttackMontageSections;
	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName> DeathMontageSections;


};
