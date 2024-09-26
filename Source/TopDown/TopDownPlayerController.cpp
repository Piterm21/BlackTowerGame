// Copyright Epic Games, Inc. All Rights Reserved.

#include "TopDownPlayerController.h"
#include "GameFramework/Pawn.h"
#include "TopDownCharacter.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilities/BasicAttackAbility.h"
#include "GameplayAbilities/ChargedAttackAbility.h"
#include "TopDownPlayerCharacter.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

ATopDownPlayerController::ATopDownPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void ATopDownPlayerController::Tick(float DeltaSeconds)
{
	//Rotate character for face the cursor
	APawn* const P = GetPawnOrSpectator();
	if (P)
	{
		FVector WorldPos;
		FVector WorldDirection;
		DeprojectMousePositionToWorld(WorldPos, WorldDirection);
		FVector LineEnd = WorldPos + WorldDirection * 50000;

		FVector CurrentLocation = P->GetActorLocation();
		FVector Intersection = FMath::LinePlaneIntersection(WorldPos, LineEnd, CurrentLocation, { 0,0,1.0f });
		FRotator3d Rotator = FRotationMatrix::MakeFromX(Intersection - CurrentLocation).Rotator();
			
		SetControlRotation(Rotator);
	}
}

UAbilitySystemComponent* ATopDownPlayerController::GetAbilitySystemComponent()
{
	if (ATopDownPlayerCharacter* TopDownPlayerCharacter = (ATopDownPlayerCharacter*)GetCharacter())
	{
		return TopDownPlayerCharacter->GetAbilitySystemComponent();
	}
	else
	{
		return nullptr;
	}
}

void ATopDownPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		//Movement
		EnhancedInputComponent->BindAction(UpAction, ETriggerEvent::Triggered, this, &ATopDownPlayerController::OnMoveUp);
		EnhancedInputComponent->BindAction(DownAction, ETriggerEvent::Triggered, this, &ATopDownPlayerController::OnMoveDown);
		EnhancedInputComponent->BindAction(LeftAction, ETriggerEvent::Triggered, this, &ATopDownPlayerController::OnMoveLeft);
		EnhancedInputComponent->BindAction(RightAction, ETriggerEvent::Triggered, this, &ATopDownPlayerController::OnMoveRight);

		//Attacks
		EnhancedInputComponent->BindAction(BasicAttackAction, ETriggerEvent::Started, this, &ATopDownPlayerController::OnBasicAttack);
		EnhancedInputComponent->BindAction(ChargeAttackAction, ETriggerEvent::Started, this, &ATopDownPlayerController::OnChargedAttackStart);
		EnhancedInputComponent->BindAction(ChargeAttackAction, ETriggerEvent::Completed, this, &ATopDownPlayerController::OnChargedAttackEnd);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ATopDownPlayerController::OnMoveUp()
{
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		ControlledPawn->AddMovementInput({ 1.0f, 0, 0 });
	}
}

void ATopDownPlayerController::OnMoveDown()
{
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		ControlledPawn->AddMovementInput({ -1.0f, 0, 0 });
	}
}

void ATopDownPlayerController::OnMoveLeft()
{
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		ControlledPawn->AddMovementInput({ 0, -1.0f, 0 });
	}
}

void ATopDownPlayerController::OnMoveRight()
{
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		ControlledPawn->AddMovementInput({ 0, 1.0f, 0 });
	}
}

void ATopDownPlayerController::OnBasicAttack()
{
	//Try to trigger basic attack ability
	if (ATopDownCharacter* TopDownCharacter = (ATopDownCharacter*)GetCharacter())
	{
		if (TopDownCharacter->GetAbilitySystemComponent()->TryActivateAbilityByClass(UBasicAttackAbility::StaticClass()))
		{
#if WITH_EDITOR && (UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT)
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Shooting allowed")));
#endif
		}
		else
		{
#if WITH_EDITOR && (UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT)
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Shooting blocked")));
#endif
		}
	}
}

void ATopDownPlayerController::OnChargedAttackStart()
{
	//Trigger InputID set for charged attack ability, starting the ability if possible
	if (ATopDownPlayerCharacter* TopDownPlayerCharacter = (ATopDownPlayerCharacter*)GetCharacter())
	{
		TopDownPlayerCharacter->GetAbilitySystemComponent()->AbilityLocalInputPressed(TopDownPlayerCharacter->ChargedAttackAbilityInputID);
	}
}

void ATopDownPlayerController::OnChargedAttackEnd()
{
	//Release InputID for charged attack ability, resulting in it's completion
	if (ATopDownPlayerCharacter* TopDownPlayerCharacter = (ATopDownPlayerCharacter*)GetCharacter())
	{
		TopDownPlayerCharacter->GetAbilitySystemComponent()->AbilityLocalInputReleased(TopDownPlayerCharacter->ChargedAttackAbilityInputID);
	}
}