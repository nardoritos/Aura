// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"

#include "AuraEffectActor.generated.h"

class UNiagaraSystem;
class UAbilitySystemComponent;
class UGameplayEffect;

UENUM(BlueprintType)
enum class EEffectApplicationPolicy : uint8
{
	ApplyOnOverlap,
	ApplyOnEndOverlap,
	DoNotApply
};

UENUM(BlueprintType)
enum class EEffectRemovalPolicy : uint8
{
	RemoveOnEndOverlap,
	DoNotRemove
};

UCLASS()
class AURA_API AAuraEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AAuraEffectActor();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass);

	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	bool bDestroyOnEffectApplication = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	bool bApplyEffectToEnemies = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EEffectApplicationPolicy InstantEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EEffectApplicationPolicy DurationEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> InfiniteGameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EEffectApplicationPolicy InfiniteEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EEffectRemovalPolicy InfiniteEffectRemovalPolicy = EEffectRemovalPolicy::RemoveOnEndOverlap;

	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Applied Effects")
	float ActorLevel = 1.f;

	UFUNCTION(BlueprintCallable)
	void SetCollisionState(bool bCollisionEnabled);

	UPROPERTY(BlueprintReadWrite, Category = "Cosmetic Effects")
	FVector Destination;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetDestinationAndAnimate(FVector InDestination);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PlayIdleAnimation();

	UPROPERTY(EditDefaultsOnly, Category = "Cosmetic Effects")
	TObjectPtr<UNiagaraSystem> PickupSystem;

	UPROPERTY(EditDefaultsOnly, Category = "Cosmetic Effects")
	TObjectPtr<USoundBase> PickupSound;
};
