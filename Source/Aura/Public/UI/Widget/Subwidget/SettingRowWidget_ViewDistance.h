// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/Subwidget/SettingRowWidget.h"
#include "SettingRowWidget_ViewDistance.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API USettingRowWidget_ViewDistance : public USettingRowWidget
{
	GENERATED_BODY()
	
public:
	
	void ReceiveInitialValue(int32 InValue);
	
	virtual void UpdateSettingValue(bool bOverrideValue = false) override;

	virtual void NextValue() override;
	virtual void PreviousValue() override;

	void SetCurrentViewDistance(int32 InViewDistance);
	int32 GetCurrentViewDistance() const { return CurrentViewDistance; }

protected:
	
	UPROPERTY()
	int32 CurrentViewDistance;

	// @param Value 0:near, 1:medium, 2:far, 3:epic, 4:cinematic
	UPROPERTY()
	TArray<int32> AvailableViewDistance = { 0, 1, 2, 3, 4};
};
