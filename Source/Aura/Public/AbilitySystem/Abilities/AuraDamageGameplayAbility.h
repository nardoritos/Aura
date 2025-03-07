// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayAbility.h"
#include "Interaction/CombatInterface.h"
#include "AuraDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	
	UAuraDamageGameplayAbility();
	

	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);

	UFUNCTION(BlueprintPure)
	float GetDamageAtLevel() const;
	float GetDamageAtLevel(int32 Level) const; // Used for Text Formatting
	
	UFUNCTION(BlueprintPure)
	float GetPropagatedDamageAtLevel() const;
	float GetPropagatedDamageAtLevel(int32 Level) const; // Used for Text Formatting
	
	UFUNCTION(BlueprintPure)
	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(
		AActor* TargetActor = nullptr,
		FVector InRadialDamageOrigin = FVector::ZeroVector,
		bool bOverrideKnockbackDirection = false,
		FVector InKnockbackDirectionOverride = FVector::ZeroVector,
		bool bOverrideDeathImpulse = false,
		FVector DeathImpulseDirectionOverride = FVector::ZeroVector,
		bool bOverridePitch = false,
		float PitchOverride = 0.f) const;

	// Activation Block Tags
	void AddDefaultActivationBlockedTags();

	// Block Abilities with Tag
	void AddDefaultBlockAbilitiesWithTag();
	
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGameplayTag DamageType;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FScalableFloat Damage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FScalableFloat PropagatedDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage|Debuff")
	float DebuffChance = 20.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage|Debuff")
	float DebuffDamage = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage|Debuff")
	float DebuffFrequency = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage|Debuff")
	float DebuffDuration = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage|Misc")
	float DeathImpulseMagnitude = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage|Knockback")
	float KnockbackForceMagnitude = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage|Knockback")
	float KnockbackChance = 0.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage|Radial")
	bool bIsRadialDamage = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage|Radial")
	float RadialDamageInnerRadius = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage|Radial")
	float RadialDamageOuterRadius = 0.f;
	
	UFUNCTION(BlueprintPure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const;

	
};
