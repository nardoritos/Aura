// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class ULevelUpInfo;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChanged, int32 /*StatValue*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnLevelChanged, int32 /*StatValue*/, bool /*bLevelUp*/);

class UAttributeSet;
class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAuraPlayerState();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	UPROPERTY(EditAnywhere)
	TObjectPtr<ULevelUpInfo> LevelUpInfo;
		
	FOnPlayerStatChanged OnXPChangedDelegate;
	FOnLevelChanged OnLevelChangedDelegate;
	FOnPlayerStatChanged OnAttributePointsChangedDelegate;
	FOnPlayerStatChanged OnSpellPointsChangedDelegate;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	FORCEINLINE int32 GetPlayerLevel() const { return Level; }	
	FORCEINLINE int32 GetPlayerXP() const { return XP; }
	FORCEINLINE int32 GetPlayerAttributePoints() const { return AttributePoints; }
	FORCEINLINE int32 GetPlayerSpellPoints() const { return SpellPoints; }

	void SetLevel(int32 InLevel);
	void SetXP(int32 InXP);
	void SetAttributePoints(int32 InAttributePoints);
	void SetSpellPoints(int32 InSpellPoints);

	void AddToLevel(int32 InLevel);
	void AddToXP(int32 InXP);
	void AddToAttributePoints(int32 InAttributePoints);
	void AddToSpellPoints(int32 InSpellPoints);
	
	
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;


private:

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Level)
	int32 Level = 1;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

	UPROPERTY(ReplicatedUsing=OnRep_XP)
	int32 XP = 0;
	
	UFUNCTION()
	void OnRep_XP(int32 OldXP);

	UPROPERTY(ReplicatedUsing=OnRep_AttributePoints)
	int32 AttributePoints = 0;
	
	UFUNCTION()
	void OnRep_AttributePoints(int32 OldAttributePoints);
	
	UPROPERTY(ReplicatedUsing=OnRep_SpellPoints)
	int32 SpellPoints = 0;
	
	UFUNCTION()
	void OnRep_SpellPoints(int32 OldSpellPoints);
};
