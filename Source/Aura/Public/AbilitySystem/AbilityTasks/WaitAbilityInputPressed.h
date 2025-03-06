// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "WaitAbilityInputPressed.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FConfirmInputPressSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCancelInputPressSignature);
/**
 * 
 */
UCLASS()
class AURA_API UWaitAbilityInputPressed : public UAbilityTask
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FConfirmInputPressSignature ConfirmInputPressed;

	UPROPERTY(BlueprintAssignable)
	FCancelInputPressSignature CancelInputPressed;
	
	UFUNCTION()
	void OnPressCallback(const FGameplayTag& ReceivedInputTag);

	virtual void Activate() override;
	
	/** Wait until the user presses the same input button for this ability's activation OR confirm button (LMB). */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UWaitAbilityInputPressed* WaitAbilityInputPressed(UGameplayAbility* OwningAbility, const FGameplayTag AbilityInputTag);

protected:

	FGameplayTag AbilityInputTag;
	FDelegateHandle DelegateHandle;
	
};
