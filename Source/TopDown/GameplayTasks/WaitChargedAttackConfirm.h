#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "WaitChargedAttackConfirm.generated.h"

UCLASS()
class UWaitChargedAttackConfirm : public UAbilityTask_WaitInputRelease
{
	GENERATED_BODY()

public:
	UWaitChargedAttackConfirm();

	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UWaitChargedAttackConfirm* WaitChargedAttackConfirm(class UGameplayAbility* OwningAbility, UStaticMesh* InWeaponMesh, float InChargeTimeMax, float InIncrementCount);

private:

	float ChargeTime = 0.0f;
	int LastChargedIncrements = 0;

	TObjectPtr<UStaticMesh> WeaponMesh;
	float ChargeTimeMax;
	float IncrementCount;
	float IncrementSize;

	TArray<TObjectPtr<UStaticMeshComponent>> SpawnedWeapons;
};