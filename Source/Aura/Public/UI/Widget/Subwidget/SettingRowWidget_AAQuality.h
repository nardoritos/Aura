// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SettingRowWidget.h"
#include "SettingRowWidget_AAQuality.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API USettingRowWidget_AAQuality : public USettingRowWidget
{
	GENERATED_BODY()
	
public:
	
	void ReceiveInitialValue(int32 InValue);
	
	virtual void UpdateSettingValue(bool bOverrideValue = false) override;

	virtual void NextValue() override;
	virtual void PreviousValue() override;

	void SetCurrentAAQuality(int32 InAAQuality);
	int32 GetCurrentAAQuality() const { return CurrentAAQuality; }

protected:
	
	UPROPERTY()
	int32 CurrentAAQuality;

	// @param Value -1:disabled (hidden by selection), 0:low, 1:medium, 2:high, 3:epic, 4:cinematic
	UPROPERTY()
	TArray<int32> AvailableAAQualityOptions = { -1, 0, 1, 2, 3, 4};
};
