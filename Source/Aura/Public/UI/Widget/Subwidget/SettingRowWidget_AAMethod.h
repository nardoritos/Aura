// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SettingRowWidget.h"
#include "SettingRowWidget_AAMethod.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API USettingRowWidget_AAMethod : public USettingRowWidget
{
	GENERATED_BODY()
	
public:
	
	void ReceiveInitialValue(int32 InValue);
	
	virtual void UpdateSettingValue(bool bOverrideValue = false) override;

	virtual void NextValue() override;
	virtual void PreviousValue() override;

	void SetCurrentAAMethod(int32 InAAMethod);
	int32 GetCurrentAAMethod() const { return CurrentAAMethod; }

protected:
	
	UPROPERTY()
	int32 CurrentAAMethod;

	// @param Value 0:none, 1:fxaa, 2:taa, 3:msaa, 4:tsr
	UPROPERTY()
	TArray<int32> AvailableAAMethodOptions = { 0, 1, 2, 3, 4};
};