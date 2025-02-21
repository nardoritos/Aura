// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();

	SpellPointsChangedDelegate.Broadcast(GetAuraPS()->GetPlayerSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetAuraASC()->AbilityStatusChange.AddLambda(
		[this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 NewLevel)
		{
			if (SelectedAbility.Ability.MatchesTagExact(AbilityTag))
			{
				SelectedAbility.Status = StatusTag;
				SelectedAbility.Level = NewLevel;
				bool bEnableSpendPointsButton = false, bShouldEnableEquipButton = false;
				ShouldEnableButtons(StatusTag, CurrentSpellPoints, bEnableSpendPointsButton, bShouldEnableEquipButton);
				
				FString AbilityDescription, NextLevelAbilityDescription;
				UAuraAbilitySystemLibrary::GetAbilityDescription(this, GetAuraASC(), SelectedAbility.Ability, SelectedAbility.Level, AbilityDescription, NextLevelAbilityDescription);
	
				SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPointsButton, bShouldEnableEquipButton, AbilityDescription, NextLevelAbilityDescription);
			}
				
			if (AbilityInfo)
			{
				FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
				Info.StatusTag = StatusTag;
				AbilityInfoDelegate.Broadcast(Info);
			}
		});

	GetAuraASC()->AbilityEquipped.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);
	
	// Binding to OnSpellPointsChanged
	GetAuraPS()->OnSpellPointsChangedDelegate.AddLambda(
		[this](int32 Points)
		{
			SpellPointsChangedDelegate.Broadcast(Points);
			CurrentSpellPoints = Points;

			bool bEnableSpendPointsButton = false, bShouldEnableEquipButton = false;

			ShouldEnableButtons(SelectedAbility.Status, CurrentSpellPoints, bEnableSpendPointsButton, bShouldEnableEquipButton);

			FString AbilityDescription, NextLevelAbilityDescription;
			UAuraAbilitySystemLibrary::GetAbilityDescription(this, GetAuraASC(), SelectedAbility.Ability, SelectedAbility.Level, AbilityDescription, NextLevelAbilityDescription);
		
			SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPointsButton, bShouldEnableEquipButton, AbilityDescription, NextLevelAbilityDescription);
		});
	
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	if (bWaitingForEquipSelection)
	{
		const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
		StopWaitingForEquipDelegate.Broadcast(SelectedAbilityType);
		bWaitingForEquipSelection = false;	
	}
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	const int32 SpellPoints = GetAuraPS()->GetPlayerSpellPoints();
	FGameplayTag AbilityStatus;

	const bool bTagValid = AbilityTag.IsValid();
	const bool bTagNone = AbilityTag.MatchesTag(GameplayTags.Abilities_None);
	const FGameplayAbilitySpec* AbilitySpec = GetAuraASC()->GetSpecFromAbilityTag(AbilityTag);
	const bool bSpecValid = AbilitySpec != nullptr; 

	int32 AbilityLevel = 0;
	if (!bTagValid || bTagNone || !bSpecValid)
	{
		AbilityStatus = GameplayTags.Abilities_Status_Locked;
	}
	else
	{
		AbilityStatus = GetAuraASC()->GetStatusFromSpec(*AbilitySpec);
		AbilityLevel = AbilitySpec->Level;
	}

	SelectedAbility.Ability = AbilityTag;
	SelectedAbility.Status = AbilityStatus;
	SelectedAbility.Level = AbilityLevel;
	bool bEnableSpendPointsButton = false, bShouldEnableEquipButton = false;

	ShouldEnableButtons(AbilityStatus, SpellPoints, bEnableSpendPointsButton, bShouldEnableEquipButton);
	
	FString AbilityDescription, NextLevelAbilityDescription;
	UAuraAbilitySystemLibrary::GetAbilityDescription(this, GetAuraASC(), SelectedAbility.Ability, SelectedAbility.Level, AbilityDescription, NextLevelAbilityDescription);
				
	SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPointsButton, bShouldEnableEquipButton, AbilityDescription, NextLevelAbilityDescription);
}

FString USpellMenuWidgetController::SpellGlobeHovered(const FGameplayTag& AbilityTag)
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	
	const bool bTagValid = AbilityTag.IsValid();
	const bool bTagNone = AbilityTag.MatchesTag(GameplayTags.Abilities_None);
	const FGameplayAbilitySpec* AbilitySpec = GetAuraASC()->GetSpecFromAbilityTag(AbilityTag);
	const bool bSpecValid = AbilitySpec != nullptr; 

	int32 AbilityLevel = 0;
	if (bTagValid && !bTagNone && bSpecValid)
	{
		AbilityLevel = AbilitySpec->Level;
	}
	
	FString AbilityDescription, EmptyString;
	UAuraAbilitySystemLibrary::GetAbilityDescription(this, GetAuraASC(), AbilityTag, AbilityLevel, AbilityDescription, EmptyString);

	return AbilityDescription;
}

void USpellMenuWidgetController::SpendPointButtonPressed()
{
	if (GetAuraASC())
		GetAuraASC()->ServerSpendSpellPoint(SelectedAbility.Ability);
}

void USpellMenuWidgetController::GlobeDeselect()
{
	if (bWaitingForEquipSelection)
	{
		const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
		StopWaitingForEquipDelegate.Broadcast(SelectedAbilityType);
		bWaitingForEquipSelection = false;
	}
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	
	SelectedAbility.Ability = GameplayTags.Abilities_None;
	SelectedAbility.Status = GameplayTags.Abilities_Status_Locked;
	SelectedAbility.Level = 0;

	SpellGlobeSelectedDelegate.Broadcast(false,false,FString(), FString());
	
}

void USpellMenuWidgetController::EquipButtonPressed()
{
	const FGameplayTag AbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;

	WaitForEquipDelegate.Broadcast(AbilityType);
	bWaitingForEquipSelection = true;

	const FGameplayTag SelectedAbilityStatus = GetAuraASC()->GetStatusFromAbilityTag(SelectedAbility.Ability); 
	if (SelectedAbilityStatus.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped))
	{
		SelectedSlot = GetAuraASC()->GetInputTagFromAbilityTag(SelectedAbility.Ability);
	}

}

void USpellMenuWidgetController::SpellRowGlobePressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType)
{
	if (!bWaitingForEquipSelection) return;
	// Check selected ability against the slot's ability type.
	// Don't equip an offensive spell in a passive slot/vice-versa

	const FGameplayTag& SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
	if (!SelectedAbilityType.MatchesTagExact(AbilityType)) return;

	GetAuraASC()->ServerEquipAbility(SelectedAbility.Ability, SlotTag);

	
}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status,
	const FGameplayTag& SlotTag, const FGameplayTag& PrevSlot)
{
	bWaitingForEquipSelection = false; 

	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	
	FAuraAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked;
	LastSlotInfo.InputTag = PrevSlot;
	LastSlotInfo.AbilityTag = GameplayTags.Abilities_None;
	// Broadcast empty info if PreviousSlot is a valid slot. Only if equipping an already equipped spell
	AbilityInfoDelegate.Broadcast(LastSlotInfo);

	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	Info.StatusTag = Status;
	Info.InputTag = SlotTag;
	AbilityInfoDelegate.Broadcast(Info);

	StopWaitingForEquipDelegate.Broadcast(AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType);
	SpellGlobeReassignedDelegate.Broadcast(AbilityTag);
	GlobeDeselect();
	
}

void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 SpellPoints,
                                                     bool& bShouldEnableSpendPointsButton, bool& bShouldEnableEquipButton)
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();

	bShouldEnableSpendPointsButton = false;
	bShouldEnableEquipButton = false;
	
	if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Equipped))
	{
		bShouldEnableEquipButton = true;
		bShouldEnableSpendPointsButton = SpellPoints > 0;
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
	{
		bShouldEnableSpendPointsButton = SpellPoints > 0;
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
	{
		bShouldEnableEquipButton = true;
	}	
}
