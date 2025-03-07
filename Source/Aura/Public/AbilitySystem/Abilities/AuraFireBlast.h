// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraFireBlast.generated.h"

class AAuraFireBall;

/**
 * 
 */
UCLASS()
class AURA_API UAuraFireBlast : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	TArray<AAuraFireBall*> SpawnFireBalls();
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "FireBlast")
	FScalableFloat MaxNumProjectiles;


private:
	
	UPROPERTY(EditDefaultsOnly, Category = "FireBlast")
	TSubclassOf<AAuraFireBall> FireBallClass;
};
