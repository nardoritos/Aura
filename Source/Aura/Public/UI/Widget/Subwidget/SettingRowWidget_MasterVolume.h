// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SettingRowWidget.h"
#include "SettingRowWidget_MasterVolume.generated.h"

class USlider;
/**
 * 
 */
UCLASS()
class AURA_API USettingRowWidget_MasterVolume : public USettingRowWidget
{
	GENERATED_BODY()

public:
	
	virtual void SetSelectedValue(float InValue) override; // Called by blueprint
	
	void ReceiveInitialValue(float InValue);
	
	virtual void UpdateSettingValue(bool bOverrideValue = false) override;

	void SetCurrentMasterVolume(float InMasterVolume);
	float GetCurrentMasterVolume() const { return CurrentMasterVolume; }

	virtual void ApplySetting() override;
	virtual void ResetToDefault() override;
	
protected:

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USlider> Slider_Value;

	UPROPERTY(BlueprintReadOnly)
	float CurrentMasterVolume;
	UPROPERTY(BlueprintReadOnly)
	float CurrentSelectedMasterVolume;
	UPROPERTY(BlueprintReadOnly)
	float CurrentAppliedMasterVolume;
};
