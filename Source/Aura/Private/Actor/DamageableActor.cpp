// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "Actor/DamageableActor.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/Aura.h"
#include "Aura/AuraLogChannels.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"


ADamageableActor::ADamageableActor()
{
	PrimaryActorTick.bCanEverTick = true;
	
	Tags.Add(FName("DamageableActor"));
	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	SetRootComponent(BoxComponent);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECC_PROJECTILE, ECR_Overlap);
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxComponent->SetGenerateOverlapEvents(true);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
}

void ADamageableActor::Die(const FVector& DeathImpulse)
{
	SetLifeSpan(LifeSpan);
	SpawnLoot();
	Multicast_HandleDeath();
}

FOnASCRegistered& ADamageableActor::GetOnASCRegisteredDelegate()
{
	return OnASCRegistered;
}

FOnDeathSignature& ADamageableActor::GetOnDeathDelegate()
{
	return OnDeathDelegate;
}

bool ADamageableActor::IsDead_Implementation() const
{
	return bDead;
}

AActor* ADamageableActor::GetAvatar_Implementation()
{
	return this;
}

void ADamageableActor::LoadActor_Implementation()
{
	if (bDead)
	{
		Destroy();
	}
}

void ADamageableActor::HighlightActor_Implementation()
{
	for (UStaticMeshComponent* Mesh : Meshes)
	{
		Mesh->SetRenderCustomDepth(true);
	}
}

void ADamageableActor::UnHighlightActor_Implementation()
{
	for (UStaticMeshComponent* Mesh : Meshes)
	{
		Mesh->SetRenderCustomDepth(false);
	}
}

UAbilitySystemComponent* ADamageableActor::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ADamageableActor::ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);

	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);

	const FGameplayEffectSpecHandle EffectSpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*EffectSpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void ADamageableActor::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultDamageableAttributes, 1.f);

	if (UAuraAttributeSet* AuraAS = Cast<UAuraAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetHealthAttribute()).AddUObject(this, &ADamageableActor::OnHealthChanged);
	}
}


void ADamageableActor::BeginPlay()
{
	Super::BeginPlay();

	InitAbilityActorInfo();
	if (bActivatesOnProximity)
	{
		BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
	}
	RegisterChildrenMeshes();
}
void ADamageableActor::RegisterChildrenMeshes()
{
	Meshes.Empty();
	Meshes.AddUnique(MeshComponent);
	MeshComponent->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	TArray<USceneComponent*> ChildrenComponents;
	MeshComponent->GetChildrenComponents(true,ChildrenComponents);
	for(USceneComponent* CurrentComp : ChildrenComponents)
	{
		if (UStaticMeshComponent* ChildMeshComponent = Cast<UStaticMeshComponent>(CurrentComp))
		{
			Meshes.AddUnique(ChildMeshComponent);
			ChildMeshComponent->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
		}
	}
}

void ADamageableActor::OnHealthChanged(const FOnAttributeChangeData& Data) const
{
	UE_LOG(LogAura, Log, TEXT("OldValue: %f, NewValue: %f"), Data.OldValue, Data.NewValue);	
}

void ADamageableActor::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	if(UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		AuraASC->AbilityActorInfoSet();

		if (HasAuthority())
		{
			InitializeDefaultAttributes();
			TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
			for (auto Pair : OnDeathAbilities)
			{
				StartupAbilities.AddUnique(Pair.Key);
			}
			AuraASC->AddCharacterAbilities(StartupAbilities);
		}

		OnASCRegistered.Broadcast(AbilitySystemComponent);
	}
}

void ADamageableActor::Multicast_HandleDeath_Implementation()
{
	bDead = true;
	if (HasAuthority())
	{
		if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent()))
		{
			FGameplayTagContainer TagContainer;
			for (auto Pair : OnDeathAbilities)
				TagContainer.AddTag(Pair.Value);
			
			AuraASC->TryActivateAbilitiesByTag(TagContainer);
		}
	}
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	for (UStaticMeshComponent* Mesh : Meshes)
	{
		Mesh->SetEnableGravity(true);
		Mesh->SetSimulatePhysics(true);
		Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		Mesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	}

	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());
	
	Dissolve();
	
	OnDeathDelegate.Broadcast(this);
}

void ADamageableActor::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor) && OtherActor->Implements<UCombatInterface>() && !OtherActor->ActorHasTag(FName("DamageableActor")))
	{
		Die(FVector::ZeroVector);
	}
}

void ADamageableActor::Dissolve()
{
	TArray<UMaterialInstanceDynamic*> DynamicMaterialInstances;
	for (UStaticMeshComponent* Mesh : Meshes)
	{
		DynamicMaterialInstances.AddUnique(Mesh->CreateDynamicMaterialInstance(0));

	}
	StartDissolveTimeline(DynamicMaterialInstances);
}
