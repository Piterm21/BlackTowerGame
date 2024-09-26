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
	//Setup ability
	UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();

	AbilityTags.AddTag(TagsManager.RequestGameplayTag("Ability.AttackHeavy"));
	BlockAbilitiesWithTag.AddTag(TagsManager.RequestGameplayTag("Ability.Attack"));
	BlockAbilitiesWithTag.AddTag(TagsManager.RequestGameplayTag("Ability.AttackHeavy"));

	CooldownGameplayEffectClass = UChargedAttackCooldown::StaticClass();

	//Find weapon asset to use when spawning projectiles/visual representation of charging
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
	//Save values for Commiting the ability, we commit the abiltity after release of the ability key, so cooldown is applied at that time
	AbilityHandle = Handle;
	AbilityActorInfo = ActorInfo;
	AbilityActivationInfo = ActivationInfo;

	ATopDownPlayerCharacter* TopDownPlayerCharacter = Cast<ATopDownPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!TopDownPlayerCharacter)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	//Create task which shows ability charging around the player and waits for ability key to be released
	UWaitChargedAttackConfirm* Task = UWaitChargedAttackConfirm::WaitChargedAttackConfirm(this, WeaponMesh, ChargeTimeMax, IncerementCount);
	Task->OnRelease.AddUniqueDynamic(this, &ThisClass::OnInputReleased);
	Task->ReadyForActivation();
}

void UChargedAttackAbility::OnInputReleased(float TimeHeld)
{
	//Commit the ability once created task triggers our callback
	if (!CommitAbility(AbilityHandle, AbilityActorInfo, AbilityActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	//"perfect release" handling, spawn double the amount of projectiles if we release within 10% of max charge 
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
		//Spawn projectiles around the character which triggered the ability, spawning is done in equal increments starting from character's facing direction
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