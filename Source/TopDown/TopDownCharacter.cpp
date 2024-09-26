// Copyright Epic Games, Inc. All Rights Reserved.

#include "TopDownCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
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

	//Setup ability system component for the character
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));

	//Setup default values for attributes
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

	//Setup basic attributes
	if (IsValid(AbilitySystemComponent))
	{
		BasicAttributes = AbilitySystemComponent->GetSet<UBasicAttributes>();
	}
}