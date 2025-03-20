// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "GameFramework/Actor.h"
#include "AuraProjectileBase.generated.h"

class UProjectileMovementComponent;
class UNiagaraSystem;

UCLASS(Abstract)
class AURA_API AAuraProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	AAuraProjectileBase();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FDamageEffectParams DamageEffectParams;

	UPROPERTY()
	TObjectPtr<USceneComponent> HomingTargetSceneComponent;

protected:
	
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	
	bool IsValidOverlap(AActor* OtherActor);
	
	virtual void OnHit();
	
	void StopSound();
	
	bool bHit = false;

	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bDestroyOnHit = false;
	
private:

	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 15.f;
		
	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactEffect;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> LoopingSound;

};
