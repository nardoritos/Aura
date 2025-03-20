// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SettingRowWidget.h"
#include "SettingRowWidget_SFXVolume.generated.h"

class USlider;
/**
 * 
 */
UCLASS()
class AURA_API USettingRowWidget_SFXVolume : public USettingRowWidget
{
	GENERATED_BODY()

public:
	
	virtual void SetSelectedValue(float InValue) override; // Called by blueprint
	
	void ReceiveInitialValue(float InValue);
	
	virtual void UpdateSettingValue(bool bOverrideValue = false) override;

	void SetCurrentSFXVolume(float InSFXVolume);
	float GetCurrentSFXVolume() const { return CurrentSFXVolume; }

	virtual void ApplySetting() override;
	virtual void ResetToDefault() override;
	
protected:

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USlider> Slider_Value;

	UPROPERTY(BlueprintReadOnly)
	float CurrentSFXVolume;
	UPROPERTY(BlueprintReadOnly)
	float CurrentSelectedSFXVolume;
	UPROPERTY(BlueprintReadOnly)
	float CurrentAppliedSFXVolume;
};
