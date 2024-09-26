#include "ChargedAttackCooldown.h"
#include "GameplayTagsManager.h"

UChargedAttackCooldown::UChargedAttackCooldown()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DurationMagnitude = { 5.0f };

	UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();
	CachedGrantedTags.AddTag(TagsManager.RequestGameplayTag("Ability.AttackHeavy.Cooldown"));
	CachedAssetTags.AddTag(TagsManager.RequestGameplayTag("Ability.AttackHeavy.Cooldown"));
}