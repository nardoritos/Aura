// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SettingRowWidget.h"
#include "SettingRowWidget_QualityPresets.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API USettingRowWidget_QualityPresets : public USettingRowWidget
{
	GENERATED_BODY()
	
public:
	
	void ReceiveInitialValue(int32 InValue);
	
	virtual void UpdateSettingValue(bool bOverrideValue = false) override;

	virtual void NextValue() override;
	virtual void PreviousValue() override;

	void SetCurrentQualityPreset(int32 InQualityPreset);
	int32 GetCurrentQualityPreset() const { return CurrentQualityPreset; }

protected:
	
	UPROPERTY()
	int32 CurrentQualityPreset;

	// @param Value -1:custom, 0:low, 1:medium, 2:high, 3:epic, 4:cinematic
	UPROPERTY()
	TArray<int32> AvailableQualityPresets = { -1, 0, 1, 2, 3, 4};
};
