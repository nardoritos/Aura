// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SettingRowWidget.h"
#include "SettingRowWidget_GlobalIlluminationQuality.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API USettingRowWidget_GlobalIlluminationQuality : public USettingRowWidget
{
	GENERATED_BODY()
	
public:
	
	void ReceiveInitialValue(int32 InValue);
	
	virtual void UpdateSettingValue(bool bOverrideValue = false) override;

	virtual void NextValue() override;
	virtual void PreviousValue() override;

	void SetCurrentGlobalIlluminationQuality(int32 InGlobalIlluminationQuality);
	int32 GetCurrentGlobalIlluminationQuality() const { return CurrentGlobalIlluminationQuality; }

protected:
	
	UPROPERTY()
	int32 CurrentGlobalIlluminationQuality;

	// @param Value -1:disabled (hidden by selection), 0:low, 1:medium, 2:high, 3:epic, 4:cinematic
	UPROPERTY()
	TArray<int32> AvailableGlobalIlluminationQualityOptions = { -1, 0, 1, 2, 3, 4};
};
