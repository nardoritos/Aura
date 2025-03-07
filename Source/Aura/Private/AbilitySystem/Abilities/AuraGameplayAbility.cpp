// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/AuraLogChannels.h"

float UAuraGameplayAbility::GetManaCostAtLevel(float InLevel) const
{
	float ManaCost = 0.f;
	if (const UGameplayEffect* CostEffect = GetCostGameplayEffect())
	{
		for (FGameplayModifierInfo Mod : CostEffect->Modifiers)
		{
			if(Mod.Attribute == UAuraAttributeSet::GetManaAttribute())
			{
				Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, ManaCost);
				break;
			}
		}	
	}

	return ManaCost;
}

float UAuraGameplayAbility::GetCooldownAtLevel(float InLevel) const
{
	float CooldownDuration = 0.f;
	if (const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
	{
		bool Result = CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel,  CooldownDuration);
		UE_LOG(LogAura, Log, TEXT("Result: %s"), Result ? *FString("true") : *FString("false"));
	}
	
	return CooldownDuration;
}
