// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actor/AuraProjectileSphere.h"
#include "AuraStandbyProjectile.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProjectileHitSignature, AAuraStandbyProjectile*, HittingProjectile, FTransform, HitLocation);

class AAuraCharacter;

UCLASS()
class AURA_API AAuraStandbyProjectile : public AAuraProjectileSphere
{
	GENERATED_BODY()

public:
	
	AAuraStandbyProjectile();

	virtual void Destroyed() override;
	
	void BindToPlayer(AAuraCharacter* InCharacter);

protected:

	UPROPERTY(BlueprintAssignable)
	FOnProjectileHitSignature OnProjectileHit;
	
	UFUNCTION(BlueprintCallable)
	void AnimationFinished();

	bool bHasFinishedAnimation = false;
	
	void UpdateHomingTarget(AActor* HomingTarget);

	UPROPERTY()
	TObjectPtr<AAuraCharacter> OwnerCharacter;

	UPROPERTY()
	FTimerHandle TimerHandle_IncreaseSpeed;
};
