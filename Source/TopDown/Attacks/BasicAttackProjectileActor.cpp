#include "BasicAttackProjectileActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "AbilitySystemComponent.h"
#include "TopDown/TopDownCharacter.h"
#include "TopDown/Attributes/BasicAttributes.h"
#include "Components/BoxComponent.h"

ABasicAttackProjectileActor::ABasicAttackProjectileActor()
{
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->InitialSpeed = 500;
	ProjectileMovementComponent->MaxSpeed = 500;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0;

	Root = CreateDefaultSubobject<UBoxComponent>(TEXT("Root"));
	RootComponent = Root;
	Root->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	Root->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	Root->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	Root->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	MeshComponent->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	ConstructorHelpers::FObjectFinder<UStaticMesh> ObjectFinder(TEXT("/Game/Weapon_Pack/Mesh/Weapons/Weapons_Kit/SM_Dagger_1"));
	MeshComponent->SetStaticMesh(ObjectFinder.Object);
	FRotator Rotation = { -90.0f, 0, 0 };
	MeshComponent->SetRelativeRotation(Rotation);
	MeshComponent->SetRelativeLocation({ -15, 0, 0 });

	FVector ProjectileBoundsMin;
	FVector ProjectileBoundsMax;
	MeshComponent->GetLocalBounds(ProjectileBoundsMin, ProjectileBoundsMax);

	FVector Bounds = Rotation.RotateVector((ProjectileBoundsMax - ProjectileBoundsMin) / 2.0f);
	Root->SetBoxExtent(Bounds);
}

void ABasicAttackProjectileActor::PostInitProperties()
{
	Super::PostInitProperties();

	OnActorBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnBeingOverlap);
	OnActorHit.AddUniqueDynamic(this, &ThisClass::OnHit);
}

void ABasicAttackProjectileActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	Lifetime -= DeltaSeconds;
	if (Lifetime <= 0.0f)
	{
		Destroy();
	}
}

void ABasicAttackProjectileActor::OnBeingOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (EffectToApply.IsValid() && OtherActor && !Cast<ABasicAttackProjectileActor>(OtherActor) && Owner != OtherActor)
	{
		if (ATopDownCharacter* TopDownCharacter = Cast<ATopDownCharacter>(OtherActor))
		{
			TopDownCharacter->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*EffectToApply.Data.Get());
#if WITH_EDITOR && (UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT)
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Health: %f"), TopDownCharacter->BasicAttributes.Get()->GetHealth()));
#endif
		}

		Destroy();
	}
}

void ABasicAttackProjectileActor::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && !Cast<ABasicAttackProjectileActor>(OtherActor) && Owner != OtherActor)
	{
		Destroy();
	}
}

void ABasicAttackProjectileActor::SetProjectileSpeed(float Speed)
{
	ProjectileMovementComponent->InitialSpeed = Speed;
	ProjectileMovementComponent->MaxSpeed = Speed;
}