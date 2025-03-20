// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/Subwidget/SettingRowWidget.h"
#include "SettingRowWidget_PostProcessingQuality.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API USettingRowWidget_PostProcessingQuality : public USettingRowWidget
{
	GENERATED_BODY()
	
public:
	
	void ReceiveInitialValue(int32 InValue);
	
	virtual void UpdateSettingValue(bool bOverrideValue = false) override;

	virtual void NextValue() override;
	virtual void PreviousValue() override;

	void SetCurrentPostProcessingQuality(int32 InPostProcessingQuality);
	int32 GetCurrentPostProcessingQuality() const { return CurrentPostProcessingQuality; }

protected:
	
	UPROPERTY()
	int32 CurrentPostProcessingQuality;

	// @param Value 0:low, 1:medium, 2:high, 3:epic, 4:cinematic
	UPROPERTY()
	TArray<int32> AvailablePostProcessingQuality = { 0, 1, 2, 3, 4};
};
