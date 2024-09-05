// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "CharacterStates.h"
#include "Interfaces/PickupInterface.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "GreystoneCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class AItem;
class UAnimMontage;
class AWeapon;
class UGreystoneOverlay;
class ASoul;
class IOnlineSessionInterface;;

UCLASS()
class ULTIMATECOURSE_API AGreystoneCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGreystoneCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void AddSouls(ASoul* Soul) override;
	virtual void AddGold(ALoot* Loot) override;


	 FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; };

	 // Multiplayer
	 IOnlineSessionPtr OnlineSessionInterface;
	 IOnlineSubsystem* OnlineSubsystem;
	 
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* GreystoneContext;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MovementAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* EquipAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* AttackAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* DodgeAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* CreateSessionAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JoinSessionAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* QuitGameAction;

	/**
	 * Callbacks for inputs
	 * 
	*/
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void EKeyPressed();
	void EquipWeapon(AWeapon* OverlappingWeapon);
	virtual void Attack() override;
	void Dodge();
	void QuitGame();

	

	// Combat
	virtual	void AttackEnd() override;
	virtual	void DodgeEnd() override;
	virtual bool CanAttack() override;
	void PlayArmingMontage(const FName& SectionName);
	bool CanDisarm();
	bool CanArm();
	UFUNCTION(BlueprintCallable)
	void Disarm();
	UFUNCTION(BlueprintCallable)
	void Arm();
	UFUNCTION(BlueprintCallable)
	void Done();
	virtual void HitReactEnd() override;
	virtual void Die_Implementation(const FName& SectionName) override;
	bool HasEnoughStamina();
	bool IsOccupied();

	// Multiplayer
	
	void CreateGameSession();
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccesful);
	void JoinGameSession();
	void OnFindSessionsComplete(bool bWasSuccesful);
	void OnJoinSessionComplete(FName JoinedSessionName, EOnJoinSessionCompleteResult::Type Result);
	bool IsValidSessionInterface();

private:	
	bool IsUnoccupied();

	void InitializeGreystoneOverlay();
	void SetHUDHealth();

	// Components 
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;
	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* EquipMontage;

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY()
	UGreystoneOverlay* GreystoneOverlay;

	// Multiplayer
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
};
