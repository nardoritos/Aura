// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/Subwidget/SettingRowWidget.h"
#include "SettingRowWidget_Resolution.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API USettingRowWidget_Resolution : public USettingRowWidget
{
	GENERATED_BODY()

public:
	
	void ReceiveInitialValue(FIntPoint InValue);
	
	virtual void UpdateSettingValue(bool bOverrideValue = false) override;

	virtual void NextValue() override;
	virtual void PreviousValue() override;

	void SetCurrentResolution(FIntPoint InResolution);
	FIntPoint GetCurrentResolution() const { return CurrentResolution; }

	void SetAvailableResolutions(TArray<FIntPoint> InAvailableResolutions);

protected:
	
	UPROPERTY()
	FIntPoint CurrentResolution;

	UPROPERTY()
	TArray<FIntPoint> ConvenientResolutions;
};
