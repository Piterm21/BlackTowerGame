// Copyright Epic Games, Inc. All Rights Reserved.

#include "TopDownCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "AbilitySystemComponent.h"
#include "Attributes/BasicAttributes.h"

ATopDownCharacter::ATopDownCharacter()
{
	// Set size capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));

	ConstructorHelpers::FObjectFinder<UDataTable> ObjectFinder(TEXT("/Game/TopDown/Attributes/BasicAttributesDefaults"));
	AttributeDefaultsDatatable = ObjectFinder.Object;

	if (IsValid(AbilitySystemComponent))
	{
		FAttributeDefaults Defaults;
		Defaults.Attributes = UBasicAttributes::StaticClass();
		Defaults.DefaultStartingTable = AttributeDefaultsDatatable;

		AbilitySystemComponent->DefaultStartingData.Add(Defaults);
	}
}

void ATopDownCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(AbilitySystemComponent))
	{
		BasicAttributes = AbilitySystemComponent->GetSet<UBasicAttributes>();
	}
}