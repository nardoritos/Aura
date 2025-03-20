// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SettingRowWidget.h"
#include "UObject/Object.h"
#include "SettingRowWidget_WindowMode.generated.h"


UCLASS()
class AURA_API USettingRowWidget_WindowMode : public USettingRowWidget
{
	GENERATED_BODY()

public:
	
	void ReceiveInitialValue(EWindowMode::Type InWindowMode);
	
	virtual void UpdateSettingValue(bool bOverrideValue = false) override;
	
	virtual void NextValue() override;
	virtual void PreviousValue() override;

	void SetCurrentWindowMode(EWindowMode::Type InWindowMode);
	EWindowMode::Type GetCurrentWindowMode() const { return CurrentWindowMode; }

protected:
	
	UPROPERTY()
	TEnumAsByte<EWindowMode::Type> CurrentWindowMode;
	
	TArray<EWindowMode::Type> WindowModes = {
		EWindowMode::Type::Fullscreen,
		EWindowMode::Type::Windowed,
		EWindowMode::Type::WindowedFullscreen
	};
	
};
