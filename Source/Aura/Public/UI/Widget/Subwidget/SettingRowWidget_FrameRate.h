// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/Subwidget/SettingRowWidget.h"
#include "SettingRowWidget_FrameRate.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API USettingRowWidget_FrameRate : public USettingRowWidget
{
	GENERATED_BODY()
	
public:
	
	void ReceiveInitialValue(int32 InValue);
	
	virtual void UpdateSettingValue(bool bOverrideValue = false) override;

	virtual void NextValue() override;
	virtual void PreviousValue() override;

	void SetCurrentFrameRate(int32 InFrameRate);
	int32 GetCurrentFrameRate() const { return CurrentFrameRate; }

protected:
	
	UPROPERTY()
	int32 CurrentFrameRate;

	// @param Values in frames
	UPROPERTY()
	TArray<int32> AvailableFrameRate = { 30, 60, 120, 144, 160, 165, 240, 360};
};
