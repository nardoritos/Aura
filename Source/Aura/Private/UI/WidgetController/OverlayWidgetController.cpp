 // Copyright (c) 2024, Nardoritos. All rights reserved.


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/AuraPlayerState.h"

 void UOverlayWidgetController::BroadcastInitialValues()
{	
	OnHealthChanged.Broadcast(GetAuraAS()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetAuraAS()->GetMaxHealth());
	OnManaChanged.Broadcast(GetAuraAS()->GetMana());
	OnMaxManaChanged.Broadcast(GetAuraAS()->GetMaxMana());
 	AttributePointsChangedDelegate.Broadcast(GetAuraPS()->GetPlayerAttributePoints());
 	SpellPointsChangedDelegate.Broadcast(GetAuraPS()->GetPlayerSpellPoints());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	GetAuraPS()->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	GetAuraPS()->OnLevelChangedDelegate.AddLambda(
		[this](int32 NewLevel, bool bLevelUp)
		{
			OnPlayerLevelChangedDelegate.Broadcast(NewLevel, bLevelUp);
		}
	);
	
	BindAttributeChange(GetAuraAS()->GetHealthAttribute(), OnHealthChanged);
	BindAttributeChange(GetAuraAS()->GetMaxHealthAttribute(), OnMaxHealthChanged);
	BindAttributeChange(GetAuraAS()->GetManaAttribute(), OnManaChanged);
	BindAttributeChange(GetAuraAS()->GetMaxManaAttribute(), OnMaxManaChanged);

	if(GetAuraASC())
	{
		GetAuraASC()->OnAbilityEquipped.AddUObject(this, &UOverlayWidgetController::OnAbilityEquipped);
		
		// If the abilities have already been given, skip Binding event
		if (GetAuraASC()->bStartupAbilitiesGiven)
			BroadcastAbilityInfo();
		else
			GetAuraASC()->OnAbilitiesGiven.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);
	
		GetAuraASC()->OnEffectAssetTagsReceived.AddLambda(
			[this](const FGameplayTagContainer& AssetTags)
			{
				for(const FGameplayTag& Tag : AssetTags)
				{
					// For example, say that Tag = Message.HealthPotion
					// "Message.HealthPotion".MatchesTag("Message") will return True, "Message".MatchesTag("Message.HealthPotion") will return False
					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
					if(Tag.MatchesTag(MessageTag))
					{
						const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
						MessageWidgetRowDelegate.Broadcast(*Row);
					}
				}
			}
		);
	}

 	// Binding to OnAttributePointsChanged 
 	GetAuraPS()->OnAttributePointsChangedDelegate.AddLambda(
		 [this](int32 Points)
		 {
			 AttributePointsChangedDelegate.Broadcast(Points);
		 }
	 );
 	
 	// Binding to OnSpellPointsChanged
 	GetAuraPS()->OnSpellPointsChangedDelegate.AddLambda(
		 [this](int32 Points)
		 {
			 SpellPointsChangedDelegate.Broadcast(Points);
		 });
}

void UOverlayWidgetController::BindAttributeChange(FGameplayAttribute Attribute,
	FOnAttributeChangedSignature& AttributeData)
{
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddLambda(
		[this, &AttributeData](const FOnAttributeChangeData& Data)
		{
			AttributeData.Broadcast(Data.NewValue);
		}
	);
}

 void UOverlayWidgetController::OnXPChanged(const int32 NewXP)
 {
 	ULevelUpInfo* LevelUpInfo = GetAuraPS()->LevelUpInfo;

 	checkf(LevelUpInfo, TEXT("Unable to find LevelUpInfo. Please fill out AuraPlayerState Blueprint."));

 	const int32 Level = LevelUpInfo->FindLevelForXP(NewXP);
 	const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num();
 	
	if (Level <= MaxLevel && Level > 0)
	{
		const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformation[Level].LevelUpRequirement;
		const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInformation[Level - 1].LevelUpRequirement;

		const int32 DeltaLevelRequirement = LevelUpRequirement - PreviousLevelUpRequirement;
		const int32 XPForThisLevel = NewXP - PreviousLevelUpRequirement;

		const float XPBarPercent = static_cast<float>(XPForThisLevel) / static_cast<float>(DeltaLevelRequirement);

		OnXPPercentChangedDelegate.Broadcast(XPBarPercent);
	}
 	
 }

 void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status,
	 const FGameplayTag& SlotTag, const FGameplayTag& PrevSlot) const
 {
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
 }
