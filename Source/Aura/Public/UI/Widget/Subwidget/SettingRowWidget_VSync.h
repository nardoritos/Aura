// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/Subwidget/SettingRowWidget.h"
#include "SettingRowWidget_VSync.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API USettingRowWidget_VSync : public USettingRowWidget
{
	GENERATED_BODY()
	
public:
	
	void ReceiveInitialValue(int32 InValue);
	
	virtual void UpdateSettingValue(bool bOverrideValue = false) override;

	virtual void NextValue() override;
	virtual void PreviousValue() override;

	void SetCurrentVSync(bool InVSync);
	bool GetCurrentVSync() const { return CurrentVSync; }

protected:
	
	UPROPERTY()
	bool CurrentVSync;

	// @param Value 0:off, 1:on
	UPROPERTY()
	TArray<bool> AvailableVSync = { false, true};
};
