// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "Character/AuraCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"
#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AAuraCharacterBase::AAuraCharacterBase()
{
 	PrimaryActorTick.bCanEverTick = false;

	BurnDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("BurnDebuffComponent");
	BurnDebuffComponent->SetupAttachment(RootComponent);
	BurnDebuffComponent->DebuffTag = FAuraGameplayTags::Get().Debuff_Burn;

	StunDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("StunDebuffComponent");
	StunDebuffComponent->SetupAttachment(RootComponent);
	StunDebuffComponent->DebuffTag = FAuraGameplayTags::Get().Debuff_Stun;
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_PROJECTILE, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
	
	SkeletalWeapon = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalWeapon");
	SkeletalWeapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	SkeletalWeapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	StaticWeapon = CreateDefaultSubobject<UStaticMeshComponent>("StaticWeapon");
	StaticWeapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	StaticWeapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	EffectAttachComponent = CreateDefaultSubobject<USceneComponent>("EffectAttachPoint");
	EffectAttachComponent->SetupAttachment(GetRootComponent());
	// Make this Comp's rotation stay independent of player, this way the effects will always face towards the player camera
	EffectAttachComponent->SetUsingAbsoluteRotation(true);
	EffectAttachComponent->SetWorldRotation(FRotator::ZeroRotator);
	
	HaloOfProtectionNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("HaloOfProtectionComp");
	HaloOfProtectionNiagaraComponent->SetupAttachment(EffectAttachComponent);

	LifeSiphonNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("LifeSiphonComp");
	LifeSiphonNiagaraComponent->SetupAttachment(EffectAttachComponent);

	ManaSiphonNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("ManaSiphonComp");
	ManaSiphonNiagaraComponent->SetupAttachment(EffectAttachComponent);
	
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAnimMontage* AAuraCharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

void AAuraCharacterBase::Die(const FVector& DeathImpulse, AActor* KillingActor)
{
	SkeletalWeapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	StaticWeapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	
	MulticastHandleDeath(DeathImpulse);
}

void AAuraCharacterBase::MulticastHandleDeath_Implementation(const FVector& DeathImpulse)
{
	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());

	if (IsValid(SkeletalWeapon->GetSkinnedAsset()))
	{
		SkeletalWeapon->SetSimulatePhysics(true);
		SkeletalWeapon->SetEnableGravity(true);
		SkeletalWeapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		SkeletalWeapon->AddImpulse(DeathImpulse * 0.1f, NAME_None, true);
	}
	if (IsValid(StaticWeapon->GetStaticMesh()))
	{
		StaticWeapon->SetSimulatePhysics(true);
		StaticWeapon->SetEnableGravity(true);
		StaticWeapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		StaticWeapon->AddImpulse(DeathImpulse);
	}
	
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->AddImpulse(DeathImpulse, NAME_None, true);
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Dissolve();
	bDead = true;

	// Force Deactivation of Niagara Debuff Systems
	BurnDebuffComponent->Deactivate();
	StunDebuffComponent->Deactivate();
	
	OnDeathDelegate.Broadcast(this);
}

void AAuraCharacterBase::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bIsStunned = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bIsStunned ? 0.f : BaseWalkSpeed;
}

void AAuraCharacterBase::OnRep_Stunned()
{
}

void AAuraCharacterBase::OnRep_Burned()
{
}

void AAuraCharacterBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraCharacterBase, bIsStunned);
	DOREPLIFETIME(AAuraCharacterBase, bIsBurned);
	DOREPLIFETIME(AAuraCharacterBase, bIsBeingShocked);
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

FVector AAuraCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Weapon) && IsValid(SkeletalWeapon))
	{
		return SkeletalWeapon->GetSocketLocation(WeaponTipSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Weapon) && IsValid(StaticWeapon))
	{
		return StaticWeapon->GetSocketLocation(WeaponTipSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_LeftHand))
	{
		return GetMesh()->GetSocketLocation(LeftHandSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_RightHand))
	{
		return GetMesh()->GetSocketLocation(RightHandSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Other))
	{
		return GetMesh()->GetSocketLocation(OtherSocketName);
	}
	
	return FVector::ZeroVector;
}

bool AAuraCharacterBase::IsDead_Implementation() const
{
	return bDead;
}

AActor* AAuraCharacterBase::GetAvatar_Implementation()
{
	return this;
	
}

TArray<FTaggedMontage> AAuraCharacterBase::GetAttackMontages_Implementation()
{
	return AttackMontages;
}

UNiagaraSystem* AAuraCharacterBase::GetBloodEffect_Implementation()
{
	return BloodEffect;
}

FTaggedMontage AAuraCharacterBase::GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag)
{
	for (FTaggedMontage TaggedMontage : AttackMontages)
	{
		if (TaggedMontage.MontageTag == MontageTag)
			return TaggedMontage;
		
	}
	return FTaggedMontage();
}

int32 AAuraCharacterBase::GetMinionCount_Implementation()
{
	return MinionCount;
}

void AAuraCharacterBase::IncrementMinionCount_Implementation(int32 Amount)
{
	MinionCount += Amount;
}

void AAuraCharacterBase::DecrementMinionCount_Implementation(int32 Amount)
{
	MinionCount -= Amount;
}

ECharacterClass AAuraCharacterBase::GetCharacterClass_Implementation()
{
	return CharacterClass;
}

FOnASCRegistered& AAuraCharacterBase::GetOnASCRegisteredDelegate()
{
	return OnASCRegistered;
}

FOnDeathSignature& AAuraCharacterBase::GetOnDeathDelegate()
{
	return OnDeathDelegate;
}

UMeshComponent* AAuraCharacterBase::GetWeapon_Implementation()
{

	// TODO: Eventually come back here if a character ever happens to swap between Skeletal or Static Weapons
	UMeshComponent* FoundMesh = nullptr;
	if (IsValid(SkeletalWeapon->GetSkinnedAsset()))
	{
		FoundMesh = SkeletalWeapon;
	}
	else if (IsValid(StaticWeapon->GetStaticMesh()))
	{
		FoundMesh = StaticWeapon;
	}

	return FoundMesh;
}

bool AAuraCharacterBase::IsBeingShocked_Implementation() const
{
	return bIsBeingShocked;
}

void AAuraCharacterBase::SetIsBeingShocked_Implementation(const bool bInIsBeingShocked)
{
	bIsBeingShocked = bInIsBeingShocked;
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
}

void AAuraCharacterBase::ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);

	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);

	const FGameplayEffectSpecHandle EffectSpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*EffectSpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
}

void AAuraCharacterBase::AddCharacterAbilities()
{
	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	
	// If not in server, does nothing
	if(!HasAuthority()) return;

	AuraASC->AddCharacterAbilities(StartupAbilities);
	AuraASC->AddCharacterPassiveAbilities(StartupPassiveAbilities);
	AuraASC->AddCharacterStartupEffects(StartupEffects);
}

void AAuraCharacterBase::Dissolve()
{
	if(IsValid(DissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicMatInst);

		StartDissolveTimeline(DynamicMatInst);
	}
	if(IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
		SkeletalWeapon->SetMaterial(0, DynamicMatInst);
		StaticWeapon->SetMaterial(0, DynamicMatInst);
		
		StartWeaponDissolveTimeline(DynamicMatInst);
	}
}
