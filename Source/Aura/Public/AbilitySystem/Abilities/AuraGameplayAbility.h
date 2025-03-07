// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input")
	FGameplayTag StartupInputTag;

	float GetManaCostAtLevel(float InLevel = 1.f) const;
	float GetCooldownAtLevel(float InLevel = 1.f) const;
	
};
