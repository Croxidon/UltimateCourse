// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/GreystoneAnimInstance.h"
#include "Characters/GreystoneCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UGreystoneAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	GreystoneCharacter = Cast<AGreystoneCharacter>(TryGetPawnOwner());
	if (GreystoneCharacter)
	{
		GreystoneCharacterMovement = GreystoneCharacter->GetCharacterMovement();
	}
	
}

void UGreystoneAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	{
		if (GreystoneCharacter)
		{
			GroundSpeed = UKismetMathLibrary::VSizeXY(GreystoneCharacterMovement->Velocity);
			isFalling = GreystoneCharacterMovement->IsFalling();
			CharacterState = GreystoneCharacter->GetCharacterState();
		}
	}
}
