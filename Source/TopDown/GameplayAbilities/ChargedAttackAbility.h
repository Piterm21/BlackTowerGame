#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ChargedAttackAbility.generated.h"

UCLASS()
class UChargedAttackAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UChargedAttackAbility();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	UFUNCTION()
	void OnInputReleased(float TimeHeld);

private:

	FGameplayAbilitySpecHandle AbilityHandle;
	const FGameplayAbilityActorInfo* AbilityActorInfo;
	FGameplayAbilityActivationInfo AbilityActivationInfo;

	const float ChargeTimeMax = 5.0f;
	const float IncerementCount = 8.0f;
	const float IncementSize = 1.0f / IncerementCount;

	TObjectPtr<UStaticMesh> WeaponMesh;
};