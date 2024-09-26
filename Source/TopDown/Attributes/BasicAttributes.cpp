#include "BasicAttributes.h"

void UBasicAttributes::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	//Destroy ownig actor if health reaches 0
	if (Attribute == GetHealthAttribute() && NewValue <= 0.0f)
	{
		GetOwningActor()->Destroy();
	}
}