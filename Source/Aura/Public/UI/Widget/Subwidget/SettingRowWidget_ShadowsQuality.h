// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/Subwidget/SettingRowWidget.h"
#include "SettingRowWidget_ShadowsQuality.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API USettingRowWidget_ShadowsQuality : public USettingRowWidget
{
	GENERATED_BODY()
	
public:
	
	void ReceiveInitialValue(int32 InValue);
	
	virtual void UpdateSettingValue(bool bOverrideValue = false) override;

	virtual void NextValue() override;
	virtual void PreviousValue() override;

	void SetCurrentShadowsQuality(int32 InShadowsQuality);
	int32 GetCurrentShadowsQuality() const { return CurrentShadowsQuality; }

protected:
	
	UPROPERTY()
	int32 CurrentShadowsQuality;

	// @param Value 0:low, 1:medium, 2:high, 3:epic, 4:cinematic
	UPROPERTY()
	TArray<int32> AvailableShadowsQuality = { 0, 1, 2, 3, 4};
};
