// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "AbilitySystem/AbilityTasks/WaitAbilityInputPressed.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"

void UWaitAbilityInputPressed::OnPressCallback(const FGameplayTag& ReceivedInputTag)
{
	UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent.Get());
	if (!Ability || !ASC)
	{
		return;
	}
	ASC->OnInputTagPressed.Remove(DelegateHandle);
	
	// We are done. Kill us so we don't keep getting broadcast messages
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		if (ReceivedInputTag.MatchesTagExact(AbilityInputTag) || ReceivedInputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
		{
			ConfirmInputPressed.Broadcast();
		}
		else
		{
			CancelInputPressed.Broadcast();
		}
	}
	EndTask();
}

void UWaitAbilityInputPressed::Activate()
{
	UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent.Get());
	if (ASC && Ability)
	{
		DelegateHandle = ASC->OnInputTagPressed.AddUObject(this, &UWaitAbilityInputPressed::OnPressCallback);
	}
}

UWaitAbilityInputPressed* UWaitAbilityInputPressed::WaitAbilityInputPressed(UGameplayAbility* OwningAbility, const FGameplayTag AbilityInputTag)
{
	UWaitAbilityInputPressed* Task = NewAbilityTask<UWaitAbilityInputPressed>(OwningAbility);
	Task->AbilityInputTag = AbilityInputTag;
	return Task;
}
