// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SettingRowWidget.h"
#include "SettingRowWidget_GlobalIlluminationMethod.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API USettingRowWidget_GlobalIlluminationMethod : public USettingRowWidget
{
	GENERATED_BODY()
	
public:
	
	void ReceiveInitialValue(int32 InValue);
	
	virtual void UpdateSettingValue(bool bOverrideValue = false) override;

	virtual void NextValue() override;
	virtual void PreviousValue() override;

	void SetCurrentGlobalIlluminationMethod(int32 InGlobalIlluminationMethod);
	int32 GetCurrentGlobalIlluminationMethod() const { return CurrentGlobalIlluminationMethod; }

protected:
	
	UPROPERTY()
	int32 CurrentGlobalIlluminationMethod;

	// @param Value 0:none, 1:lumen, 2:screenspace
	UPROPERTY()
	TArray<int32> AvailableGlobalIlluminationMethodOptions = { 0, 1, 2};
};