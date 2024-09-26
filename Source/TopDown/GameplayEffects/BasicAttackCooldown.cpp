#include "BasicAttackCooldown.h"
#include "GameplayTagsManager.h"

UBasicAttackCooldown::UBasicAttackCooldown()
{
	//Setup basic attack cooldown effect
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DurationMagnitude = { 0.25f };

	UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();
	CachedGrantedTags.AddTag(TagsManager.RequestGameplayTag("Ability.Attack.Cooldown"));
	CachedAssetTags.AddTag(TagsManager.RequestGameplayTag("Ability.Attack.Cooldown"));
}