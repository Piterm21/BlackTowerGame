#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "BasicAttackAbility.generated.h"

UCLASS()
class UBasicAttackAbility : public UGameplayAbility
{
	GENERATED_BODY()

	UBasicAttackAbility();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
};