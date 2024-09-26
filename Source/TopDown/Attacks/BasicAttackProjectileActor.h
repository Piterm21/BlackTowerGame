#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "BasicAttackProjectileActor.generated.h"

UCLASS()
class ABasicAttackProjectileActor : public AActor
{
	GENERATED_BODY()

public:

	ABasicAttackProjectileActor();

	virtual void PostInitProperties() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void OnBeingOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	FGameplayEffectSpecHandle EffectToApply;
	TObjectPtr<AActor> Owner;

	void SetProjectileSpeed(float Speed);

private:

	/** Projectile movement */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovementComponent;

	/** Modified root, used for collision/hit */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Root, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UBoxComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Parameters, meta = (AllowPrivateAccess = "true"))
	float Lifetime = 10.0f;
};