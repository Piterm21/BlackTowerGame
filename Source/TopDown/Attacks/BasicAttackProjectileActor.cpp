#include "BasicAttackProjectileActor.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "TopDown/TopDownCharacter.h"
#include "TopDown/Attributes/BasicAttributes.h"
#include "Components/BoxComponent.h"

ABasicAttackProjectileActor::ABasicAttackProjectileActor()
{
	//Movement component
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->InitialSpeed = 500;
	ProjectileMovementComponent->MaxSpeed = 500;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0;

	//Replace root with box for collisions
	Root = CreateDefaultSubobject<UBoxComponent>(TEXT("Root"));
	RootComponent = Root;
	Root->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	//Set collision type to Pawn to avoid hit events with other instances of the projectile
	Root->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	//Set reaction to block for all channels, causes hit effects to be generated for those
	Root->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	//Change collision reaction to overlap for pawns, additional logic for applying effects or doing nothing
	Root->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	//Create mesh component and set it's static mesh to asset
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	MeshComponent->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	ConstructorHelpers::FObjectFinder<UStaticMesh> ObjectFinder(TEXT("/Game/Weapon_Pack/Mesh/Weapons/Weapons_Kit/SM_Dagger_1"));
	MeshComponent->SetStaticMesh(ObjectFinder.Object);
	//Adjust rotation and location
	FRotator Rotation = { -90.0f, 0, 0 };
	MeshComponent->SetRelativeRotation(Rotation);
	MeshComponent->SetRelativeLocation({ -15, 0, 0 });

	//Update Root box extent to contain weapon mesh
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

	//Subtract from lifetime, and destroy itself when we reach 0
	Lifetime -= DeltaSeconds;
	if (Lifetime <= 0.0f)
	{
		Destroy();
	}
}

void ABasicAttackProjectileActor::OnBeingOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	//Apply gameplay effect to overlapped actor if actor is of class ATopDownCharacter and it's not the owner of the projectile
	if (EffectToApply.IsValid() && OtherActor && !Cast<ABasicAttackProjectileActor>(OtherActor) && Owner != OtherActor)
	{
		if (ATopDownCharacter* TopDownCharacter = Cast<ATopDownCharacter>(OtherActor))
		{
			TopDownCharacter->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*EffectToApply.Data.Get());
#if WITH_EDITOR && (UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT) //Debug display for hit target health
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Health: %f"), TopDownCharacter->BasicAttributes.Get()->GetHealth()));
#endif
		}

		Destroy();
	}
}

void ABasicAttackProjectileActor::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	//Destroy itself if we hit anything other than owner or other instance of the projectile
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