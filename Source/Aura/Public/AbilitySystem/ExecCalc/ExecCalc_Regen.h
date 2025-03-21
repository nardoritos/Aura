// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_Regen.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UExecCalc_Regen : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
