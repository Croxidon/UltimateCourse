// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "UltimateCourse/DebugMacros.h"



// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (IsAlive() && Hitter)
	{
		DirectionalHitReact(Hitter->GetActorLocation(), true);
	}
	else
	{
		DirectionalHitReact(ImpactPoint, false);
	}

	PlayGetHitSound(ImpactPoint);
	SpawnGetHitParticles(ImpactPoint);
}



// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (ArmedWeapon && ArmedWeapon->GetWeaponBox())
	{
		ArmedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		ArmedWeapon->ActorsToIgnore.Empty();	
		
	}
}

void ABaseCharacter::HitReactEnd()
{

}

void ABaseCharacter::Attack()
{
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
	{
		CombatTarget = nullptr;
	}
}

void ABaseCharacter::Die_Implementation(const FName& SectionName)
{
	Tags.Add(FName("Dead"));

	// Section Name means which side took hit from
	if (const EDeathPose* FoundDeathPose = SectionToDeathPoseMap.Find(SectionName))
	{
		DeathPose = *FoundDeathPose;
	}
	SetLifeSpan(DeathLifeSpan);
	SetCapsuleCollision(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetWeaponCollisionEnabled(ECollisionEnabled::Type::NoCollision);

}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint, bool IsAlive)
{
	const FVector Forward = GetActorForwardVector();
	const FVector AttackDirection = (ImpactPoint - GetActorLocation()).GetSafeNormal2D();


	const double CosTheta = FVector::DotProduct(Forward, AttackDirection);
	double Theta = FMath::Acos(CosTheta);
	Theta = FMath::RadiansToDegrees(Theta);

	// if points down, Theta should be negative
	const FVector CrossProd = FVector::CrossProduct(Forward, AttackDirection);

	if (CrossProd.Z < 0)
	{
		Theta *= -1.f;
	}

	FName Section("Back");


	if (Theta >= -45.f && Theta < 45.f)
	{
		Section = FName("Front");

	}

	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName("Left");

	}

	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = FName("Right");

	}

	if (IsAlive)
	{
		PlayHitReactMontage(Section);
	}

	else
	{
		Die(Section);
	}
}

void ABaseCharacter::PlayGetHitSound(const FVector& ImpactPoint)
{
	if (GetHitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			GetHitSound,
			ImpactPoint
		);
	}
}

void ABaseCharacter::SpawnGetHitParticles(const FVector& ImpactPoint)
{
	if (GetHitParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			this,
			GetHitParticles,
			ImpactPoint
		);
	}
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (Attributes)
	{
		Attributes->ReceiveDamage(DamageAmount);
	}
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}


}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames)
{
	if (SectionNames.Num() <= 0) return -1;
	const int32 MaxSectionIndex{ SectionNames.Num() - 1 };
	const int32 Selection{ FMath::RandRange(0, MaxSectionIndex) };
	PlayMontageSection(Montage, SectionNames[Selection]);
	return Selection;
}

int32 ABaseCharacter::PlayAttackMontage()
{
	return PlayRandomMontageSection(AttackMontage, AttackMontageSections);
}

void ABaseCharacter::PlayDodgeMontage()
{
	PlayMontageSection(DodgeMontage, FName("Default"));
}

void ABaseCharacter::StopAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Stop(0.25, AttackMontage);
	}
}

FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if (CombatTarget == nullptr) return FVector();
	
	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
	const FVector Location = GetActorLocation();

	FVector TargetToMe = (Location - CombatTargetLocation).GetSafeNormal();
	TargetToMe *= WarpToTargetDistance;
//	DRAW_SPHERE_SingleFrame(CombatTargetLocation + TargetToMe);
	return CombatTargetLocation + TargetToMe;
}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (CombatTarget)
	{
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}

int32 ABaseCharacter::PlayDeathMontage()
{
	return PlayRandomMontageSection(DeathMontage, DeathMontageSections);
}

void ABaseCharacter::SetCapsuleCollision(ECollisionEnabled::Type OnOff)
{
	GetCapsuleComponent()->SetCollisionEnabled(OnOff);
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

bool ABaseCharacter::IsAlive()
{
	return Attributes && Attributes->IsAlive();
}

void ABaseCharacter::AttackEnd()
{

}

void ABaseCharacter::DodgeEnd()
{
}
