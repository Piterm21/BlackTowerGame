#include "WaitChargedAttackConfirm.h"
#include "AbilitySystemComponent.h"

UWaitChargedAttackConfirm::UWaitChargedAttackConfirm()
{
	//Enable ticking so we can update amount of spawned weapons around the character
	bTickingTask = true;
}

void UWaitChargedAttackConfirm::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	ChargeTime += DeltaTime;
	float ChargeTimeForCalculations = ChargeTime;

	//Limit the charge time used for calculations to charge time max, so we don't go over maximum number of increments when displaying feedback for the player
	if (ChargeTimeForCalculations > ChargeTimeMax)
	{
		ChargeTimeForCalculations = ChargeTimeMax;
	}

	float ChargeFraction = ChargeTimeForCalculations / ChargeTimeMax;
	int ChargedIncrements = ChargeFraction / IncrementSize;
	float RotationIncrement = 360.0f / (int)ChargedIncrements;

	//If amount of increments changed update amount of components and their position/rotation
	if (LastChargedIncrements != ChargedIncrements)
	{
		int IncrementIndex = 0;

		auto SetRotationLocationForMeshComponent = [&RotationIncrement, &IncrementIndex](UStaticMeshComponent* MeshComponent)
			{
				FRotator RotationToSet = { -90.0f, RotationIncrement * (float)IncrementIndex, 0.0f };
				MeshComponent->SetRelativeRotation(RotationToSet);
				FRotator RotationAroundCenter = { 0,RotationIncrement * (float)IncrementIndex, 0 };
				MeshComponent->SetRelativeLocation(RotationAroundCenter.RotateVector({ 50.0f,0,0 }));
			};

		//Update already spawned components to their new positions
		for (TObjectPtr<UStaticMeshComponent> SpawnedWeapon : SpawnedWeapons)
		{
			SetRotationLocationForMeshComponent(SpawnedWeapon);
			IncrementIndex++;
		}

		//Create new components as needed
		for (; IncrementIndex < ChargedIncrements; IncrementIndex++)
		{
			UStaticMeshComponent* NewWeapon = NewObject<UStaticMeshComponent>(Ability->GetAvatarActorFromActorInfo(), FName(FString::Printf(TEXT("StaticMeshComponent_%d"), IncrementIndex)));
			NewWeapon->AttachToComponent(Ability->GetAvatarActorFromActorInfo()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			NewWeapon->RegisterComponent();

			SetRotationLocationForMeshComponent(NewWeapon);
			NewWeapon->SetStaticMesh(WeaponMesh);

			SpawnedWeapons.Add(NewWeapon);
		}

		LastChargedIncrements = ChargedIncrements;
	}
}

void UWaitChargedAttackConfirm::OnDestroy(bool bInOwnerFinished)
{
	//Remove all spawned components on destroy
	for (TObjectPtr<UStaticMeshComponent> SpawnedWeapon : SpawnedWeapons)
	{
		SpawnedWeapon.Get()->DestroyComponent();
	}

	SpawnedWeapons.Empty();

	Super::OnDestroy(bInOwnerFinished);
}

UWaitChargedAttackConfirm* UWaitChargedAttackConfirm::WaitChargedAttackConfirm(class UGameplayAbility* OwningAbility, UStaticMesh* InWeaponMesh, float InChargeTimeMax, float InIncrementCount)
{
	//Create and return new instance of the task
	UWaitChargedAttackConfirm* Task = NewAbilityTask<UWaitChargedAttackConfirm>(OwningAbility);
	Task->WeaponMesh = InWeaponMesh;
	Task->ChargeTimeMax = InChargeTimeMax;
	Task->IncrementCount = InIncrementCount;
	Task->IncrementSize = 1.0f / Task->IncrementCount;

	return Task;
}