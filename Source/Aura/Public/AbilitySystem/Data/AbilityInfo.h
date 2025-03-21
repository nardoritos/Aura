// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"
#include "Engine/DataAsset.h"
#include "AbilityInfo.generated.h"

class UGameplayAbility;

UENUM(BlueprintType)
enum EAbilityDescriptionAdditionalDetails
{
	None,
	NumProjectiles,
	NumPropagatedTargets,
	HealAmount,
	Duration
};

USTRUCT(BlueprintType)
struct FAuraAbilityInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AbilityTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag InputTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag StatusTag = FGameplayTag();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag CooldownTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag DamageTypeTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AbilityType = FGameplayTag();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UTexture2D> Icon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UMaterialInterface> BackgroundMaterial = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (MultiLine="true"))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (MultiLine="true"))
	FText NextDescription;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (MultiLine="true"))
	FText LockedDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (MultiLine="true"))
	TMap<int32, FText> SpecificLevelDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (MultiLine="true"))
	TMap<TEnumAsByte<EAbilityDescriptionAdditionalDetails>, FScalableFloat> AdditionalDetails;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 LevelRequirement = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> Ability;

	FText GetDescription(const int32 InLevel) const;
	
	float GetSpecialAttribute(const int32 Level, const EAbilityDescriptionAdditionalDetails AttributeToFind) const;
	
};

