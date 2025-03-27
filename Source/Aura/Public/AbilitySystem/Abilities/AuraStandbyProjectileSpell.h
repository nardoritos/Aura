// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AuraProjectileSpell.h"
#include "AuraStandbyProjectileSpell.generated.h"


class AAuraStandbyProjectile;
/**
 * 
 */
UCLASS()
class AURA_API UAuraStandbyProjectileSpell : public UAuraProjectileSpell
{
	GENERATED_BODY()

protected:

	
	
	UFUNCTION(BlueprintCallable, Category = "Projectile|StandbySpell")
	TArray<AAuraStandbyProjectile*> SpawnProjectiles(AActor* AttachToActor = nullptr);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Spell")
	bool bDrawDebug = false;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Spell")
	int32 DebugProjectiles = 1;
};
