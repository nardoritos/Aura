// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AuraPassiveAbility.h"
#include "AuraRegenerativePassiveAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraRegenerativePassiveAbility : public UAuraPassiveAbility
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Passive|Regen")
	FScalableFloat RegenAmount;

	
};
