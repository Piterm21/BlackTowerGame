// Copyright Epic Games, Inc. All Rights Reserved.

#include "TopDownPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "TopDownCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilities/BasicAttackAbility.h"
#include "GameplayAbilities/ChargedAttackAbility.h"
#include "TopDown/Attacks/BasicAttackProjectileActor.h"
#include "TopDown/GameplayEffects/Damage.h"
#include "TopDown/TopDownPlayerCharacter.h"

#pragma optimize( "", off )

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

ATopDownPlayerController::ATopDownPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void ATopDownPlayerController::Tick(float DeltaSeconds)
{
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

void ATopDownPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
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
		EnhancedInputComponent->BindAction(UpAction, ETriggerEvent::Triggered, this, &ATopDownPlayerController::OnMoveUp);
		EnhancedInputComponent->BindAction(DownAction, ETriggerEvent::Triggered, this, &ATopDownPlayerController::OnMoveDown);
		EnhancedInputComponent->BindAction(LeftAction, ETriggerEvent::Triggered, this, &ATopDownPlayerController::OnMoveLeft);
		EnhancedInputComponent->BindAction(RightAction, ETriggerEvent::Triggered, this, &ATopDownPlayerController::OnMoveRight);

		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &ATopDownPlayerController::OnBasicAttack);

		EnhancedInputComponent->BindAction(ChargeAttackAction, ETriggerEvent::Started, this, &ATopDownPlayerController::OnChargedAttackStart);
		EnhancedInputComponent->BindAction(ChargeAttackAction, ETriggerEvent::Completed, this, &ATopDownPlayerController::OnChargedAttackEnd);

		// Setup touch input events
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &ATopDownPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &ATopDownPlayerController::OnTouchTriggered);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &ATopDownPlayerController::OnTouchReleased);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &ATopDownPlayerController::OnTouchReleased);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ATopDownPlayerController::OnInputStarted()
{
	StopMovement();
}

// Triggered every frame when the input is held down
void ATopDownPlayerController::OnSetDestinationTriggered()
{
	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}
	
	// Move towards mouse pointer or touch
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		//FVector CurrentLocation = ControlledPawn->GetActorLocation();

		//CachedDestination.Z = CurrentLocation.Z;

		//FRotator3d Rotator = FRotationMatrix::MakeFromX(CachedDestination - CurrentLocation).Rotator();
		//FRotator CurrentRotation = ControlledPawn->GetActorRotation();
		//FRotator Delta = CurrentRotation - Rotator;

		//SetControlRotation(Rotator);
		//ControlledPawn->AddActorWorldRotation(WorldDirection, 1.0, false);
	}
}

void ATopDownPlayerController::OnSetDestinationReleased()
{
	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		// We move there and spawn some particles
		//UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		//UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	FollowTime = 0.f;
}

// Triggered every frame when the input is held down
void ATopDownPlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void ATopDownPlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}

#define SPEED 10

void ATopDownPlayerController::OnMoveUp()
{
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		ControlledPawn->AddMovementInput({ SPEED,0,0 });
	}
}

void ATopDownPlayerController::OnMoveDown()
{
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		ControlledPawn->AddMovementInput({ -1 * SPEED,0,0 });
	}
}

void ATopDownPlayerController::OnMoveLeft()
{
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		ControlledPawn->AddMovementInput({ 0, -1 * SPEED,0 });
	}
}

void ATopDownPlayerController::OnMoveRight()
{
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		ControlledPawn->AddMovementInput({ 0, 1 * SPEED,0 });
	}
}

void ATopDownPlayerController::OnBasicAttack()
{
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
	if (ATopDownPlayerCharacter* TopDownPlayerCharacter = (ATopDownPlayerCharacter*)GetCharacter())
	{
		TopDownPlayerCharacter->GetAbilitySystemComponent()->AbilityLocalInputPressed(TopDownPlayerCharacter->ChargedAttackAbilityInputID);
	}
}

void ATopDownPlayerController::OnChargedAttackEnd()
{
	if (ATopDownPlayerCharacter* TopDownPlayerCharacter = (ATopDownPlayerCharacter*)GetCharacter())
	{
		TopDownPlayerCharacter->GetAbilitySystemComponent()->AbilityLocalInputReleased(TopDownPlayerCharacter->ChargedAttackAbilityInputID);
	}
}

#pragma optimize( "", on )