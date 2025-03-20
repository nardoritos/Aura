// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SettingRowWidget.h"
#include "SettingRowWidget_MusicVolume.generated.h"

class USlider;
/**
 * 
 */
UCLASS()
class AURA_API USettingRowWidget_MusicVolume : public USettingRowWidget
{
	GENERATED_BODY()

public:
	
	virtual void SetSelectedValue(float InValue) override; // Called by blueprint
	
	void ReceiveInitialValue(float InValue);
	
	virtual void UpdateSettingValue(bool bOverrideValue = false) override;

	void SetCurrentMusicVolume(float InMusicVolume);
	float GetCurrentMusicVolume() const { return CurrentMusicVolume; }

	virtual void ApplySetting() override;
	virtual void ResetToDefault() override;
	
protected:

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USlider> Slider_Value;

	UPROPERTY(BlueprintReadOnly)
	float CurrentMusicVolume;
	UPROPERTY(BlueprintReadOnly)
	float CurrentSelectedMusicVolume;
	UPROPERTY(BlueprintReadOnly)
	float CurrentAppliedMusicVolume;
};
