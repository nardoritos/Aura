// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "UI/Widget/SettingWidget.h"
#include "UMG.h"
#include "SettingsInfo.h"
#include "Game/AuraGameUserSettings.h"
#include "UI/Widget/Subwidget/SettingRowWidget.h"

void USettingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SettingsInfo == nullptr) return;

	Box_SettingsList->ClearChildren();
	for (FAuraSettingsInfo CurrentSetting : SettingsInfo->Settings)
	{
		
		USettingRowWidget* SettingRowWidget = NewObject<USettingRowWidget>(this, CurrentSetting.SettingClass);
		Box_SettingsList->AddChildToVerticalBox(SettingRowWidget);
		SettingRowWidget->SetSettingInfo(CurrentSetting);
		SettingRowWidget->OnValueChanged.AddUObject(this, &ThisClass::OnSettingValueChanged);
		SettingsMap.Add(CurrentSetting.SettingType, SettingRowWidget);
		
		
		UAuraGameUserSettings* AuraGameUserSettings = CastChecked<UAuraGameUserSettings>(GEngine->GetGameUserSettings());
		AuraGameUserSettings->RegisterNewSetting(CurrentSetting.SettingType, SettingRowWidget);

		
	}
}

void USettingWidget::ApplySettings()
{
	for (auto Setting : SettingsDirtyState)
	{
		if (Setting.Value)
		{
			SettingsMap[Setting.Key]->ApplySetting();
		}
	}
	UAuraGameUserSettings* AuraGameUserSettings = CastChecked<UAuraGameUserSettings>(GEngine->GetGameUserSettings());
	AuraGameUserSettings->ApplySettings(false);
	UpdateButtonsState(false);
}

void USettingWidget::ResetToDefaults()
{
	for (auto Setting : SettingsDirtyState)
	{
		if (Setting.Value)
		{
			SettingsMap[Setting.Key]->ResetToDefault();
		}
	}
	UpdateButtonsState(false);
}

void USettingWidget::OnSettingValueChanged(ESettingType SettingType, bool bIsDifferentThanCurrentlyAppliedValue)
{
	SettingsDirtyState.Add(SettingType, bIsDifferentThanCurrentlyAppliedValue);

	for (auto Setting : SettingsDirtyState)
	{
		if (Setting.Value)
		{
			UpdateButtonsState(true);
			return;
		}
	}
	UpdateButtonsState(false);
}

