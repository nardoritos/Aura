// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura/AuraLogChannels.h"
#include "Game/AuraSaveGame.h"
#include "Interaction/PlayerInterface.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
}

void UAuraAbilitySystemComponent::AddCharacterAbilitiesFromSaveData(UAuraSaveGame* SaveData)
{
	for (const FSavedAbility& SavedAbility : SaveData->SavedAbilities)
	{
		const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
		const TSubclassOf<UGameplayAbility> LoadedAbilityClass = SavedAbility.GameplayAbility;

		FGameplayAbilitySpec LoadedAbilitySpec = FGameplayAbilitySpec(LoadedAbilityClass, SavedAbility.AbilityLevel);

		LoadedAbilitySpec.GetDynamicSpecSourceTags().AddTag(SavedAbility.AbilitySlot);
		LoadedAbilitySpec.GetDynamicSpecSourceTags().AddTag(SavedAbility.AbilityStatus);

		// We want to only GiveAbility() if it's an Offensive or Passive Ability
		// If it's a Passive Ability AND it was saved as equipped, we also want to Activate it once, so it's effect become available
		if (!SavedAbility.AbilityType.MatchesTagExact(GameplayTags.Abilities_Type_None))
		{
			GiveAbility(LoadedAbilitySpec);
			if (SavedAbility.AbilityType.MatchesTagExact(GameplayTags.Abilities_Type_Passive) && SavedAbility.AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Equipped))
				TryActivateAbility(LoadedAbilitySpec.Handle);
		}
		
	}
	bStartupAbilitiesGiven = true;
	OnAbilitiesGiven.Broadcast();
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	if (bStartupAbilitiesGiven) return;
	
	for(TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{

		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if(const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(AuraAbility->StartupInputTag);
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
			GiveAbility(AbilitySpec);
		}
	}
	bStartupAbilitiesGiven = true;
	OnAbilitiesGiven.Broadcast();
	
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(
	const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for(TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilities)
	{

		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
		GiveAbilityAndActivateOnce(AbilitySpec);
		
	}
}

void UAuraAbilitySystemComponent::AddCharacterStartupEffects(const TArray<TSubclassOf<UGameplayEffect>>& StartupEffects)
{
	if (bStartupEffectsGiven) return;
	
	for (TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
	{
		FGameplayEffectSpecHandle AbilitySpecHandle = MakeOutgoingSpec(GameplayEffect, 1, MakeEffectContext());
		if (AbilitySpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle EffectHandle = ApplyGameplayEffectSpecToSelf(*AbilitySpecHandle.Data.Get());
		}
	}

	bStartupEffectsGiven = true;
}

void UAuraAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if(!InputTag.IsValid()) return;

	OnInputTagPressed.Broadcast(InputTag);
	FScopedAbilityListLock ActiveScopeLock(*this);
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	
	for(FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// if (HasMatchingGameplayTag(GameplayTags.Abilities_WaitingExecution))
		// {
		// 	if (AbilitySpec.Ability->AbilityTags.HasTagExact(GameplayTags.Abilities_WaitingExecution) && AbilitySpec.IsActive())
		// 	{
		// 		// Execute Ability in WaitingExecution Status
		// 		if (InputTag.MatchesTagExact(GameplayTags.InputTag_LMB) || AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		// 		{
		// 			AbilitySpecInputPressed(AbilitySpec);
		// 			const TArray<UGameplayAbility*> AbilityInstances = AbilitySpec.GetAbilityInstances();
		// 			for (const UGameplayAbility* Ability : AbilityInstances)
		// 			{
		// 				InvokeReplicatedEvent(
		// 					EAbilityGenericReplicatedEvent::InputPressed,
		// 					AbilitySpec.Handle,
		// 					Ability->GetCurrentActivationInfo().GetActivationPredictionKey());
		// 				return;
		// 			}
		// 		}
		// 		else 
		// 		{
		// 			// If it was any other ability button, Cancel this current Ability
		// 			
		// 			const FGameplayAbilityActorInfo* ActorInfo = AbilitySpec.Ability->GetCurrentActorInfo();
		// 			const FGameplayAbilityActivationInfo ActivationInfo = AbilitySpec.Ability->GetCurrentActivationInfo();
		// 			AbilitySpec.Ability->CancelAbility(
		// 				AbilitySpec.Handle,
		// 				ActorInfo,
		// 				ActivationInfo,
		// 				true);
		// 			
		// 		}
		// 	}
		// 	
		// }
		// else
		// {
			if(AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag) && AbilitySpec.IsActive())
			{
				AbilitySpecInputReleased(AbilitySpec);
				const TArray<UGameplayAbility*> AbilityInstances = AbilitySpec.GetAbilityInstances();
				for (const UGameplayAbility* Ability : AbilityInstances)
				{
					InvokeReplicatedEvent(
						EAbilityGenericReplicatedEvent::InputPressed,
						AbilitySpec.Handle,
						Ability->GetCurrentActivationInfo().GetActivationPredictionKey());
					return;
				}
			}
		// }
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if(!InputTag.IsValid()) return;
	
	FScopedAbilityListLock ActiveScopeLock(*this);
	
	for(FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if(AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if(!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if(!InputTag.IsValid()) return;
	
	FScopedAbilityListLock ActiveScopeLock(*this);

	for(FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if(AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag) && AbilitySpec.IsActive())
		{
			AbilitySpecInputReleased(AbilitySpec);
			const TArray<UGameplayAbility*> AbilityInstances = AbilitySpec.GetAbilityInstances();
			for (const UGameplayAbility* Ability : AbilityInstances)
			{
				InvokeReplicatedEvent(
					EAbilityGenericReplicatedEvent::InputReleased,
					AbilitySpec.Handle,
					Ability->GetCurrentActivationInfo().GetActivationPredictionKey());
				return;
			}
		}
	}
}

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogAura, Error, TEXT("Failed to execute delegate in [%hs]"), __FUNCTION__);
		}
	}
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->GetAssetTags())
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
			{
				return Tag;
			}
		}	
	}
	
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag Tag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
		{
			return Tag;
		}
	}	
	
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag StatusTag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (StatusTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status"))))
		{
			return StatusTag;
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetStatusFromSpec(*Spec);
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetSlotTagFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetInputTagFromSpec(*Spec);
	}
	return FGameplayTag();
}

bool UAuraAbilitySystemComponent::SlotIsEmpty(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilityHasSlot(AbilitySpec, Slot))
			return false;
	}
	return true;
}

bool UAuraAbilitySystemComponent::AbilityHasSlot(const FGameplayAbilitySpec& Spec, const FGameplayTag& Slot)
{
	return Spec.GetDynamicSpecSourceTags().HasTagExact(Slot);
}

bool UAuraAbilitySystemComponent::AbilityHasAnySlot(const FGameplayAbilitySpec& Spec)
{
	return Spec.GetDynamicSpecSourceTags().HasTag(FGameplayTag::RequestGameplayTag(FName("InputTag")));
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecWithSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopeLock(*this);

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilityHasSlot(AbilitySpec, Slot))
		{
			return &AbilitySpec;
		}
	}
	return nullptr;
}

bool UAuraAbilitySystemComponent::IsPassiveAbility(const FGameplayAbilitySpec& Spec) const
{
	const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	const FGameplayTag AbilityTag = GetAbilityTagFromSpec(Spec);
	const FAuraAbilityInfo& Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	const FGameplayTag AbilityType = Info.AbilityType;
	return AbilityType.MatchesTag(FAuraGameplayTags::Get().Abilities_Type_Passive);
}

void UAuraAbilitySystemComponent::AssignAbilityToSlot(FGameplayAbilitySpec& Spec, const FGameplayTag& Slot)
{
	ClearSlot(&Spec);
	Spec.GetDynamicSpecSourceTags().AddTag(Slot);
	
}

void UAuraAbilitySystemComponent::MulticastActivatePassiveEffect_Implementation(const FGameplayTag& AbilityTag,
	bool bActivate)
{
	OnActivatePassiveEffect.Broadcast(AbilityTag, bActivate);
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->GetAssetTags())
		{
			if (Tag.MatchesTag(AbilityTag))
			{
				return &AbilitySpec;
			}
		}
	}
	return nullptr;
}

int32 UAuraAbilitySystemComponent::GetLevelFromAbilityTag(const FGameplayTag& AbilityTag)
{
	int32 Level = 0;
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		Level = AbilitySpec->Level;
	}
	return Level;
}

void UAuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
		{
			ServerUpgradeAttribute(AttributeTag);
		}	
	}
}

void UAuraAbilitySystemComponent::UpdateAbilityStatuses(int32 Level)
{
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	for (const FAuraAbilityInfo& Info : AbilityInfo->AbilityInformation)
	{
		if (!Info.AbilityTag.IsValid()) continue;
		if (Level < Info.LevelRequirement) continue;
		if (GetSpecFromAbilityTag(Info.AbilityTag) == nullptr)
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability, 1);
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(FAuraGameplayTags::Get().Abilities_Status_Eligible);
			GiveAbility(AbilitySpec);
			MarkAbilitySpecDirty(AbilitySpec);
			ClientUpdateAbilityStatus(Info.AbilityTag, FAuraGameplayTags::Get().Abilities_Status_Eligible, 1);
		}
		
	}
	
}

void UAuraAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	if(FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		if (GetAvatarActor()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);
		}
		
		const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
		FGameplayTag Status = GetStatusFromSpec(*AbilitySpec);
		if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
		{
			AbilitySpec->GetDynamicSpecSourceTags().RemoveTag(GameplayTags.Abilities_Status_Eligible);
			AbilitySpec->GetDynamicSpecSourceTags().AddTag(GameplayTags.Abilities_Status_Unlocked);
			Status = GameplayTags.Abilities_Status_Unlocked;
		}
		else if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Equipped) || Status.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
		{
			AbilitySpec->Level += 1;
		}
		ClientUpdateAbilityStatus(AbilityTag, Status, AbilitySpec->Level);
		MarkAbilitySpecDirty(*AbilitySpec);
	}
}

void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1.f;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);

	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
	}
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                                     const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	OnEffectAssetTagsReceived.Broadcast(TagContainer);
	

}

void UAuraAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& SlotTag)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
		const FGameplayTag& PrevSlot = GetInputTagFromSpec(*AbilitySpec);
		const FGameplayTag& Status = GetStatusFromSpec(*AbilitySpec);

		const bool bStatusValid = Status == GameplayTags.Abilities_Status_Equipped || Status == GameplayTags.Abilities_Status_Unlocked;
		if (bStatusValid)
		{
			// Handle Activation/Deactivation for Passive Abilities

			if (!SlotIsEmpty(SlotTag)) // There is an Ability in this slot already, Deactivate and clear its slot.
			{
				// If new ability is the same as current ability, return early
				FGameplayAbilitySpec* SpecWithSlot = GetSpecWithSlot(SlotTag);
				if (SpecWithSlot)
				{
					if (AbilityTag.MatchesTagExact(GetAbilityTagFromSpec(*SpecWithSlot)))
					{
						ClientEquipAbility(AbilityTag, GameplayTags.Abilities_Status_Equipped, SlotTag, PrevSlot);
						return;
					}

					if(IsPassiveAbility(*SpecWithSlot))
					{
						MulticastActivatePassiveEffect(GetAbilityTagFromSpec(*SpecWithSlot), false);
						OnDeactivatePassiveAbility.Broadcast(GetAbilityTagFromSpec(*SpecWithSlot));
					}

					// We want to clear the slot regardless of being Passive or Offensive
					// Additionally, we also want to set the Former Ability's Status to Unlocked
					SpecWithSlot->GetDynamicSpecSourceTags().RemoveTag(GetStatusFromSpec(*SpecWithSlot));
					SpecWithSlot->GetDynamicSpecSourceTags().AddTag(GameplayTags.Abilities_Status_Unlocked);
					
					ClearSlot(SpecWithSlot);
					MarkAbilitySpecDirty(*SpecWithSlot);
				}
			}
			
			if (!AbilityHasAnySlot(*AbilitySpec)) // Ability doesn't have a Slot yet (unactive)
			{
				if (IsPassiveAbility(*AbilitySpec))
				{
					TryActivateAbility(AbilitySpec->Handle);
					MulticastActivatePassiveEffect(AbilityTag, true);
					
				}
				AbilitySpec->GetDynamicSpecSourceTags().RemoveTag(GetStatusFromSpec(*AbilitySpec));
				AbilitySpec->GetDynamicSpecSourceTags().AddTag(GameplayTags.Abilities_Status_Equipped);
				
			}
			
			AssignAbilityToSlot(*AbilitySpec, SlotTag);
			MarkAbilitySpecDirty(*AbilitySpec);
			
		}
		ClientEquipAbility(AbilityTag, GameplayTags.Abilities_Status_Equipped, SlotTag, PrevSlot);

	}
}

void UAuraAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& Status, const FGameplayTag& SlotTag, const FGameplayTag& PreviousSlot)
{
	OnAbilityEquipped.Broadcast(AbilityTag, Status, SlotTag, PreviousSlot);
}

void UAuraAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec* Spec)
{
	const FGameplayTag Slot = GetInputTagFromSpec(*Spec);
	Spec->GetDynamicSpecSourceTags().RemoveTag(Slot);
}

void UAuraAbilitySystemComponent::ClearAbilitiesOfSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (AbilityHasSlot(Spec, Slot))
		{
			ClearSlot(&Spec);
		}
	}
		
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		OnAbilitiesGiven.Broadcast();
	}
}

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& StatusTag, int32 AbilityLevel)
{
	OnAbilityStatusChanged.Broadcast(AbilityTag, StatusTag, AbilityLevel);
}

