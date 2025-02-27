// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "AbilitySystem/Data/AbilityInfo.h"

#include "AuraGameplayTags.h"
#include "Aura/AuraLogChannels.h"

FString FAuraAbilityInfo::GetDescription(const int32 InLevel) const
{
	if (SpecificLevelDescription.Contains(InLevel))
	{
		return *SpecificLevelDescription.Find(InLevel);
	}
	return Description;
}

float FAuraAbilityInfo::GetSpecialAttribute(const int32 Level, const EAbilityDescriptionAdditionalDetails AttributeToFind) const
{
	if (AdditionalDetails.Contains(AttributeToFind))
		return AdditionalDetails[AttributeToFind].GetValueAtLevel(Level);
	return -999.f;
}

FAuraAbilityInfo UAbilityInfo::FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	for (auto Info : AbilityInformation)
	{
		if (Info.AbilityTag == AbilityTag)
		{
			return Info;
		}
		
	}
	if (bLogNotFound)
	{
		UE_LOG(LogAura, Error, TEXT("Can't find Info for AbilityTag [%s] on AbilityInfo [%s]"), *AbilityTag.ToString(), *GetNameSafe(this));
	}

	return FAuraAbilityInfo();
}
