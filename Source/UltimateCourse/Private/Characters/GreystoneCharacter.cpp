// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/GreystoneCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/AttributeComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Items/Soul.h"
#include "Items/Loot.h"
#include "Animation/AnimMontage.h"
#include "HUD/GreystoneHUD.h"
#include "HUD/GreystoneOverlay.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
#include "Kismet/KismetSystemLibrary.h"



// Sets default values
 AGreystoneCharacter::AGreystoneCharacter() :
	 CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this,&ThisClass::OnCreateSessionComplete)),
	 FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	 JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this,&ThisClass::OnJoinSessionComplete))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("View Camera"));
	ViewCamera->SetupAttachment(CameraBoom);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	

}

// Called when the game starts or when spawned
void AGreystoneCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			InputSystem->AddMappingContext(GreystoneContext, 0);
		}
	}

	Tags.Add(FName("EngageableTarget"));

	InitializeGreystoneOverlay();
	
}

// Called every frame
void AGreystoneCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Attributes && GreystoneOverlay)
	{
		Attributes->RegenStamina(DeltaTime);
		GreystoneOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

// Called to bind functionality to input
void AGreystoneCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &AGreystoneCharacter::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGreystoneCharacter::Look);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AGreystoneCharacter::Jump);
	EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &AGreystoneCharacter::EKeyPressed);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AGreystoneCharacter::Attack);
	EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &AGreystoneCharacter::Dodge);
	EnhancedInputComponent->BindAction(CreateSessionAction, ETriggerEvent::Triggered, this, &AGreystoneCharacter::CreateGameSession);
	EnhancedInputComponent->BindAction(JoinSessionAction, ETriggerEvent::Triggered, this, &AGreystoneCharacter::JoinGameSession);
	EnhancedInputComponent->BindAction(QuitGameAction, ETriggerEvent::Triggered, this, &AGreystoneCharacter::QuitGame);


}

void AGreystoneCharacter::Jump()
{
	if (IsUnoccupied())
	{
	Super::Jump();

	}
}



float AGreystoneCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHUDHealth();
	return DamageAmount;
}


void AGreystoneCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint,Hitter);
	SetWeaponCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	if (Attributes && Attributes->IsAlive())
	{
		ActionState = EActionState::EAS_HitReaction;
	}
	

}

void AGreystoneCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void AGreystoneCharacter::AddSouls(ASoul* Soul)
{
	if (Attributes)
	{
		Attributes->AddSouls(Soul->GetSouls());
		if (GreystoneOverlay)
		{
			GreystoneOverlay->SetSouls(Attributes->GetSouls());
		}
	}
}

void AGreystoneCharacter::AddGold(ALoot* Loot)
{
	if (Attributes)
	{
		Attributes->AddGold(Loot->GetGold());
		if (GreystoneOverlay)
		{
			GreystoneOverlay->SetGold(Attributes->GetGold());
		}
	}
}

void AGreystoneCharacter::Move(const FInputActionValue& Value)
{

	if (ActionState != EActionState::EAS_Unoccupied) return;
	const FVector2D MovementVector = Value.Get<FVector2D>();
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.Y);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void AGreystoneCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>();

	AddControllerPitchInput(LookVector.Y);
	AddControllerYawInput(LookVector.X);
}

void AGreystoneCharacter::EKeyPressed()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		if (ArmedWeapon)
		{
			ArmedWeapon->Destroy();
		}
		EquipWeapon(OverlappingWeapon);
	}
	else
	{
		if (CanDisarm())
		{
			PlayArmingMontage(FName("Disarm"));
			CharacterState = ECharacterState::ECS_Unequipped;
			ActionState = EActionState::EAS_ArmingWeapon;
			
		}
		else if (CanArm())
		{
			PlayArmingMontage(FName("Arm"));
			CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
			ActionState = EActionState::EAS_ArmingWeapon;
		}
	}
}

void AGreystoneCharacter::EquipWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName("FX_Sword_HiltBase"), this, this);
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	ArmedWeapon = Weapon;
	Weapon = nullptr;
	
}

void AGreystoneCharacter::Attack()
{
	Super::Attack();
	
	if (CanAttack())
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void AGreystoneCharacter::Dodge()
{
	if (IsOccupied() || !HasEnoughStamina()) return;
	PlayDodgeMontage();
	ActionState = EActionState::EAS_Dodging;
	if (Attributes && GreystoneOverlay)
	{
		Attributes->UseStamina(Attributes->GetDodgeCost());
		GreystoneOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void AGreystoneCharacter::QuitGame()
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}

bool AGreystoneCharacter::HasEnoughStamina()
{
	return Attributes && Attributes->GetStamina() > Attributes->GetDodgeCost();
}

bool AGreystoneCharacter::IsOccupied()
{
	return ActionState != EActionState::EAS_Unoccupied;
}

void AGreystoneCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AGreystoneCharacter::DodgeEnd()
{
	Super::DodgeEnd();
	ActionState = ActionState = EActionState::EAS_Unoccupied;
}

bool AGreystoneCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied && CharacterState != ECharacterState::ECS_Unequipped;
}

void AGreystoneCharacter::PlayArmingMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}


bool AGreystoneCharacter::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped &&
		ArmedWeapon;
}

bool AGreystoneCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped &&
		ArmedWeapon;
}

void AGreystoneCharacter::Disarm()
{
	if (ArmedWeapon)
	{
		ArmedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void AGreystoneCharacter::Arm()
{
	if (ArmedWeapon)
	{
		ArmedWeapon->AttachMeshToSocket(GetMesh(), FName("FX_Sword_HiltBase"));
	}
}

void AGreystoneCharacter::Done()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AGreystoneCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AGreystoneCharacter::Die_Implementation(const FName& SectionName)
{
	Super::Die_Implementation(SectionName);
	ActionState = EActionState::EAS_Dead;
	/* Commented out since implemented inside ABP 
	int32 DeathMontagePose = PlayDeathMontage();
	 switch (DeathMontagePose)
	{
	case 0:
		DeathPose = EDeathPose::EDP_ToLeft;
		break;
	case 1:
		DeathPose = EDeathPose::EDP_ToRight;
		break;
	case 2:
		DeathPose = EDeathPose::EDP_ToFront;
		break;
	case 3:
		DeathPose = EDeathPose::EDP_ToBack;
		break;
	default:
		break;
	}
	*/
}

bool AGreystoneCharacter::IsUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

void AGreystoneCharacter::InitializeGreystoneOverlay()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		AGreystoneHUD* GreystoneHUD = Cast<AGreystoneHUD>(PlayerController->GetHUD());
		if (GreystoneHUD)
		{
			GreystoneOverlay = GreystoneHUD->GetGreystoneOverlay();
			if (GreystoneOverlay && Attributes)
			{
				GreystoneOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				GreystoneOverlay->SetStaminaBarPercent(1.f);
				GreystoneOverlay->SetGold(0);
				GreystoneOverlay->SetSouls(0);
			}
		}
	}
}


void AGreystoneCharacter::SetHUDHealth()
{
	if (GreystoneOverlay && Attributes)
	{
		GreystoneOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}



void AGreystoneCharacter::CreateGameSession()
{
	if (!IsValidSessionInterface()) return;

	FNamedOnlineSession* ExistingSession = OnlineSessionInterface->GetNamedSession(NAME_GameSession);

	if (ExistingSession != nullptr)
	{
		OnlineSessionInterface->DestroySession(NAME_GameSession);
	}

	OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
	SessionSettings->NumPublicConnections = 12;
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bUsesPresence = true;
	SessionSettings->bUseLobbiesIfAvailable = true;
	SessionSettings->Set(FName("MatchType"), FString("FreeForAll"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);



	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *SessionSettings);
}

void AGreystoneCharacter::OnCreateSessionComplete(FName SessionName, bool bWasSuccesful)
{
	FString string;
	if (bWasSuccesful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(TEXT("Created session %s"), *SessionName.ToString()));
			
		}

		string = FString::Printf(TEXT("Created session %s"), *SessionName.ToString());
		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel(FString("Game/Maps/Domadia?listen"));

		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Failed to create session")));

		}

		string = FString(TEXT("Failed to create session."));
	}
	
}

void AGreystoneCharacter::JoinGameSession()
{
	if (!IsValidSessionInterface()) return;

	//Find game sessions

	OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	
	OnlineSessionInterface->FindSessions(*(GetWorld()->GetFirstLocalPlayerFromController()->GetPreferredUniqueNetId()), SessionSearch.ToSharedRef());
}

void AGreystoneCharacter::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (!IsValidSessionInterface()) return;

	for (FOnlineSessionSearchResult Result : SessionSearch->SearchResults)
	{
		FString Id = Result.GetSessionIdStr();
		FString User = Result.Session.OwningUserName;
		FString MatchType;
		Result.Session.SessionSettings.Get(FName("MatchType"), MatchType);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Red,
				FString::Printf(TEXT("Id: %s, User: %s"), *Id, *User)
			);
		}

		if (MatchType == FString("FreeForAll"))  // Corrected parenthesis
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					15.f,
					FColor::Red,
					FString::Printf(TEXT("Joining match type: %s"), *MatchType)
				);
			}

			OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);
			OnlineSessionInterface->JoinSession(*(GetWorld()->GetFirstLocalPlayerFromController()->GetPreferredUniqueNetId()), NAME_GameSession, Result);

		}
	}
}

void AGreystoneCharacter::OnJoinSessionComplete(FName JoinedSessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!IsValidSessionInterface()) return;

	FString Address;
	if (OnlineSessionInterface->GetResolvedConnectString(NAME_GameSession, Address))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Yellow,
				FString::Printf(TEXT("Connect string: %s"), *Address)
			);
		}

		APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
		if (PlayerController)
		{
			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		}
	}
}

bool AGreystoneCharacter::IsValidSessionInterface()
{
	if (!OnlineSessionInterface)
	{
		IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
		if (Subsystem)
		{
			OnlineSessionInterface = Subsystem->GetSessionInterface();
		}
	}
	return OnlineSessionInterface.IsValid();
}
