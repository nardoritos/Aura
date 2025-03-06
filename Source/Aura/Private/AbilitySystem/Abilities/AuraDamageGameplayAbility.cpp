// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Interaction/CombatInterface.h"

UAuraDamageGameplayAbility::UAuraDamageGameplayAbility()
{
	AddDefaultActivationBlockedTags();
	AddDefaultBlockAbilitiesWithTag();
}

void UAuraDamageGameplayAbility::AddDefaultActivationBlockedTags()
{
	ActivationBlockedTags.AddTag(FAuraGameplayTags::Get().Debuff_Stun);
}

void UAuraDamageGameplayAbility::AddDefaultBlockAbilitiesWithTag()
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	BlockAbilitiesWithTag.AddTag(GameplayTags.Abilities_Attack);
	BlockAbilitiesWithTag.AddTag(GameplayTags.Abilities_Fire_FireBolt);
	BlockAbilitiesWithTag.AddTag(GameplayTags.Abilities_Lightning_Electrocute);
	BlockAbilitiesWithTag.AddTag(GameplayTags.Abilities_Arcane_ArcaneShards);
}

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	const FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);

	const float DamageMagnitude = Damage.GetValueAtLevel(GetAbilityLevel());
	
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, DamageType, DamageMagnitude);
		
	
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(),
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));

}

FTaggedMontage UAuraDamageGameplayAbility::GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const
{
	if (TaggedMontages.Num() > 0)
	{
		const int32 Selection = FMath::RandRange(0, TaggedMontages.Num() - 1);
		return TaggedMontages[Selection];
	}
	return FTaggedMontage();
}

float UAuraDamageGameplayAbility::GetDamageAtLevel() const
{
	return Damage.GetValueAtLevel(GetAbilityLevel());
}

float UAuraDamageGameplayAbility::GetDamageAtLevel(int32 Level) const
{
	return Damage.GetValueAtLevel(Level);
}

float UAuraDamageGameplayAbility::GetPropagatedDamageAtLevel() const
{
	return Damage.GetValueAtLevel(GetAbilityLevel());
}

float UAuraDamageGameplayAbility::GetPropagatedDamageAtLevel(int32 Level) const
{
	return Damage.GetValueAtLevel(Level);
}

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor,
	FVector InRadialDamageOrigin, bool bOverrideKnockbackDirection, FVector InKnockbackDirectionOverride,
	bool bOverrideDeathImpulse, FVector DeathImpulseDirectionOverride, bool bOverridePitch, float PitchOverride) const
{
	FDamageEffectParams Params;
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.DamageGameplayEffectClass = DamageEffectClass;
	Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	Params.AbilityLevel = GetAbilityLevel();
	Params.DamageType = DamageType;
	Params.DebuffChance = DebuffChance;
	Params.DebuffDamage = DebuffDamage;
	Params.DebuffDuration = DebuffDuration;
	Params.DebuffFrequency = DebuffFrequency;
	Params.DeathImpulseMagnitude = DeathImpulseMagnitude;
	Params.KnockbackForceMagnitude = KnockbackForceMagnitude;
	Params.KnockbackChance = KnockbackChance;

	if (IsValid(TargetActor))
	{
		FRotator Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
		const FVector ToTarget = Rotation.Vector();
		if (bOverridePitch)
		{
			Rotation.Pitch = PitchOverride;
		}
		if (!bOverrideKnockbackDirection)
		{
			Params.KnockbackForce = ToTarget * KnockbackForceMagnitude;
		}
		if (!bOverrideDeathImpulse)
		{
			Params.DeathImpulse = ToTarget * DeathImpulseMagnitude;
		}
			
	}
	if (bOverrideKnockbackDirection)
	{
		InKnockbackDirectionOverride.Normalize();
		Params.KnockbackForce = InKnockbackDirectionOverride * KnockbackForceMagnitude;
		if (bOverridePitch)
		{
			FRotator KnockbackRotation = InKnockbackDirectionOverride.Rotation();
			KnockbackRotation.Pitch = PitchOverride;
			Params.KnockbackForce = KnockbackRotation.Vector() * KnockbackForceMagnitude;
		}
	}

	if (bOverrideDeathImpulse)
	{
		DeathImpulseDirectionOverride.Normalize();
		Params.DeathImpulse = DeathImpulseDirectionOverride * DeathImpulseMagnitude;
		if (bOverridePitch)
		{
			FRotator DeathImpulseRotation = DeathImpulseDirectionOverride.Rotation();
			DeathImpulseRotation.Pitch = PitchOverride;
			Params.DeathImpulse =  DeathImpulseRotation.Vector() * DeathImpulseMagnitude;
		}
	}

	if (bIsRadialDamage)
	{
		Params.bIsRadialDamage = bIsRadialDamage;
		Params.RadialDamageOrigin = InRadialDamageOrigin;
		Params.RadialDamageInnerRadius = RadialDamageInnerRadius;
		Params.RadialDamageOuterRadius = RadialDamageOuterRadius;
	}
	return Params;
}

