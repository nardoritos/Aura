// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "AbilitySystem/ExecCalc/ExecCalc_Regen.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"

void UExecCalc_Regen::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                            FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	
	// Target to Apply Heal
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	
	// Get Heal set by Caller Magnitude
	auto RegenerableTags = GameplayTags.RegenerableTags;
	TMap<FGameplayTag, float> HealsToApply;
	
	for (const auto RegenTag : RegenerableTags)
	{
		const float Heal = Spec.GetSetByCallerMagnitude(RegenTag, false);
		if (Heal > 0)
		{
			HealsToApply.Add(RegenTag, Heal);
		}
	}
	for (auto Pair : HealsToApply)
	{
		FGameplayModifierEvaluatedData EvaluatedData;
		if (Pair.Key == GameplayTags.Attributes_Meta_IncomingHealthRegen)
		{
			EvaluatedData = FGameplayModifierEvaluatedData(
				UAuraAttributeSet::GetIncomingHealthRegenAttribute(),
				EGameplayModOp::Additive,
				Pair.Value);
	
			OutExecutionOutput.AddOutputModifier(EvaluatedData);
		}
		
		if (Pair.Key == GameplayTags.Attributes_Meta_IncomingManaRegen)
		{
			EvaluatedData = FGameplayModifierEvaluatedData(
				UAuraAttributeSet::GetIncomingManaRegenAttribute(),
				EGameplayModOp::Additive,
				Pair.Value);
			
			OutExecutionOutput.AddOutputModifier(EvaluatedData);
		}
	}
}
