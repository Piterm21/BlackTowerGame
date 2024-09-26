#include "BasicAttributes.h"

void UBasicAttributes::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	if (Attribute == GetHealthAttribute() && NewValue <= 0.0f)
	{
		GetOwningActor()->Destroy();
	}
}