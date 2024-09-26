#include "TopDownPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilities/BasicAttackAbility.h"
#include "GameplayAbilities/ChargedAttackAbility.h"

ATopDownPlayerCharacter::ATopDownPlayerCharacter()
{
	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

void ATopDownPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Grant basic attack to the player
	FGameplayAbilitySpec AbilitySpec = { UBasicAttackAbility::StaticClass(), 1, INDEX_NONE, this };
	AbilitySystemComponent->GiveAbility(AbilitySpec);

	//Grant charged attack to the player, triggered by Ability Input ID
	AbilitySpec = { UChargedAttackAbility::StaticClass(), 1, ChargedAttackAbilityInputID, this };
	AbilitySystemComponent->GiveAbility(AbilitySpec);
}