// Copyright (c) 2024, Nardoritos. All rights reserved.

#include "UI/Widget/Subwidget/SettingRowWidget.h"
#include "UMG.h"


void USettingRowWidget::UpdateSettingDisplay()
{
	Text_SettingName->SetText(SettingInfo.SettingName);
}

void USettingRowWidget::UpdateSettingValue(bool bOverrideValue)
{
	// Implement in Children
}

void USettingRowWidget::SetAppliedValue(int32 InAppliedValue)
{
	CurrentAppliedValueIndex = InAppliedValue;
}

void USettingRowWidget::SetSettingInfo(const FAuraSettingsInfo& InSettingInfo)
{
	SettingInfo = InSettingInfo;
	UpdateSettingDisplay();
}

void USettingRowWidget::NextValue()
{
	if (CurrentSelectedValueIndex >= AvailableOptions - 1)
	{
		CurrentSelectedValueIndex = 0;
	}
	else
	{
		CurrentSelectedValueIndex++;
	}
	UpdateSettingValue();
}

void USettingRowWidget::PreviousValue()
{
	if (CurrentSelectedValueIndex <= 0)
	{
		CurrentSelectedValueIndex = AvailableOptions - 1;
	}
	else
	{
		CurrentSelectedValueIndex--;	
	}
	UpdateSettingValue();
}

void USettingRowWidget::SetSelectedValue(float InValue)
{
}

float USettingRowWidget::GetSelectedValue()
{
	return 0.f;
}

void USettingRowWidget::ApplySetting()
{
	CurrentAppliedValueIndex = CurrentSelectedValueIndex;
	OnValueApplied.Broadcast();
}

void USettingRowWidget::ResetToDefault()
{
	CurrentSelectedValueIndex = CurrentAppliedValueIndex;
	UpdateSettingValue(true);
}

