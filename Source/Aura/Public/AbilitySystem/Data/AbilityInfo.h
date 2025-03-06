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
	NumPropagatedTargets
};

USTRUCT(BlueprintType)
struct FAuraAbilityInfo
{
	GENERATED_BODY()

	UPROPERTY(EditdefaultsOnly, BlueprintReadOnly)
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
	FString Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (MultiLine="true"))
	FString Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (MultiLine="true"))
	FString NextDescription;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (MultiLine="true"))
	FString LockedDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (MultiLine="true"))
	TMap<int32 , FString> SpecificLevelDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (MultiLine="true"))
	TMap<TEnumAsByte<EAbilityDescriptionAdditionalDetails>, FScalableFloat> AdditionalDetails;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 LevelRequirement = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> Ability;

	FString GetDescription(const int32 InLevel) const;
	
	// Num Projectiles
	float GetSpecialAttribute(const int32 Level, const EAbilityDescriptionAdditionalDetails AttributeToFind) const;
	
};

/**
 * 
 */
UCLASS()
class AURA_API UAbilityInfo : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Info")
	TArray<FAuraAbilityInfo> AbilityInformation;

	FAuraAbilityInfo FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound = false) const;

};
