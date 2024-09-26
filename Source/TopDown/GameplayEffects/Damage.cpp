#include "Damage.h"
#include "TopDown/Attributes/BasicAttributes.h"
#include "TopDown/TopDownCharacter.h"

UDamage::UDamage()
{
	//Setup damage effect
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo ModifierInfo;
	ModifierInfo.Attribute = UBasicAttributes::GetHealthAttribute();
	ModifierInfo.ModifierOp = EGameplayModOp::Type::Additive;
	ModifierInfo.ModifierMagnitude = { -10.0f };

	Modifiers.Add(ModifierInfo);
}