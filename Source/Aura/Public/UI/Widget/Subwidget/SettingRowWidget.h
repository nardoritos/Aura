// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SettingsInfo.h"
#include "UObject/Object.h"
#include "SettingRowWidget.generated.h"

class UTextBlock;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnValueChangedDelegate, ESettingType /* Setting Type */, bool /*bIsValueDifferentThanCurrentAppliedValue*/);
DECLARE_MULTICAST_DELEGATE(FOnValueAppliedDelegate);

UCLASS()
class AURA_API USettingRowWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	void UpdateSettingDisplay();
	virtual void UpdateSettingValue(bool bOverrideValue = false);

	void SetAppliedValue(int32 InAppliedValue);
	void SetSettingInfo(const FAuraSettingsInfo& InSettingInfo);
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
	virtual void NextValue();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	virtual void PreviousValue();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	virtual void SetSelectedValue(float InValue);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Settings")
	virtual float GetSelectedValue();
	
	virtual void ApplySetting();
	virtual void ResetToDefault();
	
	FOnValueChangedDelegate OnValueChanged;
	FOnValueAppliedDelegate OnValueApplied;

	UFUNCTION(BlueprintImplementableEvent)
	void Enable();
	
	UFUNCTION(BlueprintImplementableEvent)
	void Disable();

	
protected:

	int32 AvailableOptions;
	
	FAuraSettingsInfo SettingInfo;
	
	int32 CurrentSelectedValueIndex;
	int32 CurrentAppliedValueIndex;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_SettingName;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_SettingValue;

	
};
