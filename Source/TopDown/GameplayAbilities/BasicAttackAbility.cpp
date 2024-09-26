#include "BasicAttackAbility.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagsManager.h"
#include "TopDown/GameplayEffects/BasicAttackCooldown.h"
#include "TopDown/TopDownPlayerCharacter.h"
#include "TopDown/Attacks/BasicAttackProjectileActor.h"
#include "TopDown/GameplayEffects/Damage.h"

UBasicAttackAbility::UBasicAttackAbility()
{
	UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();

	AbilityTags.AddTag(TagsManager.RequestGameplayTag("Ability.Attack"));

	CooldownGameplayEffectClass = UBasicAttackCooldown::StaticClass();
}

void UBasicAttackAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
) 
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	ATopDownPlayerCharacter* TopDownPlayerCharacter = Cast<ATopDownPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!TopDownPlayerCharacter)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	ABasicAttackProjectileActor* Projectile = GetWorld()->SpawnActor<ABasicAttackProjectileActor>(TopDownPlayerCharacter->GetActorLocation(), TopDownPlayerCharacter->GetActorRotation());
	Projectile->Owner = TopDownPlayerCharacter;

	FGameplayEffectContextHandle EffectContext = TopDownPlayerCharacter->GetAbilitySystemComponent()->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle EffectHandle = TopDownPlayerCharacter->GetAbilitySystemComponent()->MakeOutgoingSpec(UDamage::StaticClass(), 1, EffectContext);
	if (EffectHandle.IsValid())
	{
		Projectile->EffectToApply = EffectHandle;
	}
}