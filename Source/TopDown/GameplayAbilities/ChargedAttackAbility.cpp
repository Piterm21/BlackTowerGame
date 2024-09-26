#include "ChargedAttackAbility.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagsManager.h"
#include "TopDown/GameplayEffects/ChargedAttackCooldown.h"
#include "TopDown/TopDownPlayerCharacter.h"
#include "TopDown/Attacks/BasicAttackProjectileActor.h"
#include "TopDown/GameplayEffects/Damage.h"
#include "TopDown/GameplayTasks/WaitChargedAttackConfirm.h"

UChargedAttackAbility::UChargedAttackAbility()
{
	UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();

	AbilityTags.AddTag(TagsManager.RequestGameplayTag("Ability.AttackHeavy"));
	BlockAbilitiesWithTag.AddTag(TagsManager.RequestGameplayTag("Ability.Attack"));
	BlockAbilitiesWithTag.AddTag(TagsManager.RequestGameplayTag("Ability.AttackHeavy"));

	CooldownGameplayEffectClass = UChargedAttackCooldown::StaticClass();

	ConstructorHelpers::FObjectFinder<UStaticMesh> ObjectFinder(TEXT("/Game/Weapon_Pack/Mesh/Weapons/Weapons_Kit/SM_Dagger_1"));
	WeaponMesh = ObjectFinder.Object;
}

void UChargedAttackAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
) 
{
	AbilityHandle = Handle;
	AbilityActorInfo = ActorInfo;
	AbilityActivationInfo = ActivationInfo;

	ATopDownPlayerCharacter* TopDownPlayerCharacter = Cast<ATopDownPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!TopDownPlayerCharacter)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	UWaitChargedAttackConfirm* Task = UWaitChargedAttackConfirm::WaitChargedAttackConfirm(this, WeaponMesh, ChargeTimeMax, IncerementCount);
	Task->OnRelease.AddUniqueDynamic(this, &ThisClass::OnInputReleased);
	Task->ReadyForActivation();
}

void UChargedAttackAbility::OnInputReleased(float TimeHeld)
{
	if (!CommitAbility(AbilityHandle, AbilityActorInfo, AbilityActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	if (FMath::Abs(ChargeTimeMax - TimeHeld) <= (ChargeTimeMax * 0.1f))
	{
		TimeHeld = ChargeTimeMax * 2.0f;
	}
	else if (TimeHeld > ChargeTimeMax)
	{
		TimeHeld = ChargeTimeMax;
	}

	float ChargeFraction = TimeHeld / ChargeTimeMax;
	int ChargedIncrements = ChargeFraction / IncementSize;
	float RotationIncrement = 360.0f / (int)ChargedIncrements;
	int IncrementIndex = 0;

	if (ATopDownPlayerCharacter* TopDownPlayerCharacter = Cast<ATopDownPlayerCharacter>(GetAvatarActorFromActorInfo()))
	{
		for (; IncrementIndex < ChargedIncrements; IncrementIndex++)
		{
			FRotator RotationAroundCenter = { 0,RotationIncrement * (float)IncrementIndex, 0 };
			FRotator SpawnRotation = TopDownPlayerCharacter->GetActorRotation() + RotationAroundCenter;

			ABasicAttackProjectileActor* Projectile = GetWorld()->SpawnActor<ABasicAttackProjectileActor>(TopDownPlayerCharacter->GetActorLocation() + SpawnRotation.RotateVector({ 50.0f,0,0 }), SpawnRotation);
			Projectile->Owner = TopDownPlayerCharacter;

			FGameplayEffectContextHandle EffectContext = TopDownPlayerCharacter->GetAbilitySystemComponent()->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			FGameplayEffectSpecHandle EffectHandle = TopDownPlayerCharacter->GetAbilitySystemComponent()->MakeOutgoingSpec(UDamage::StaticClass(), 1, EffectContext);
			if (EffectHandle.IsValid())
			{
				Projectile->EffectToApply = EffectHandle;
			}

		}
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}