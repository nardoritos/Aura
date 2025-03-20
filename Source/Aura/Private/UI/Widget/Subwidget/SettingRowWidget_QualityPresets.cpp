// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "UI/Widget/Subwidget/SettingRowWidget_QualityPresets.h"

#include "Aura/AuraLogChannels.h"
#include "Components/TextBlock.h"

void USettingRowWidget_QualityPresets::ReceiveInitialValue(int32 InValue)
{
	CurrentSelectedValueIndex = AvailableQualityPresets.Find(InValue);
	SetAppliedValue(CurrentSelectedValueIndex);
	
	SetCurrentQualityPreset(InValue);
	AvailableOptions = AvailableQualityPresets.Num();

	UE_LOG(LogAura, Log, TEXT("SettingRowWidget_QualityPresets has %i options available, with the initial option selected being %i") ,AvailableOptions, CurrentSelectedValueIndex)

}

#define LOCTEXT_NAMESPACE "UMG"

void USettingRowWidget_QualityPresets::UpdateSettingValue(bool bOverrideValue)
{
	Super::UpdateSettingValue(bOverrideValue);

	if (bOverrideValue)
	{
		CurrentQualityPreset = AvailableQualityPresets[CurrentSelectedValueIndex];
	}
	FText QualityPresetText;

	switch (CurrentQualityPreset)
	{
	case -1:
		QualityPresetText = FText(LOCTEXT("Settings.QualityPreset.Custom", "Custom"));
		break;
	case 0:
		QualityPresetText = FText(LOCTEXT("Settings.QualityPreset.Low", "Low"));
		break;
	case 1:
		QualityPresetText = FText(LOCTEXT("Settings.QualityPreset.Medium", "Medium"));
		break;
	case 2:
		QualityPresetText = FText(LOCTEXT("Settings.QualityPreset.High", "High"));
		break;
	case 3:
		QualityPresetText = FText(LOCTEXT("Settings.QualityPreset.Epic", "Epic"));
		break;
	case 4:
		QualityPresetText = FText(LOCTEXT("Settings.QualityPreset.Cinematic", "Cinematic"));
		break;
	default:
		QualityPresetText = FText::FromString("Undefined");
		break;
	}
	Text_SettingValue->SetText(QualityPresetText);
}

#undef LOCTEXT_NAMESPACE



void USettingRowWidget_QualityPresets::NextValue()
{
	if (CurrentSelectedValueIndex >= AvailableOptions - 1)
	{
		CurrentSelectedValueIndex = 1;
	}
	else
	{
		CurrentSelectedValueIndex++;
	}
	UpdateSettingValue();
	
	int32 NextQualityPreset = AvailableQualityPresets[CurrentSelectedValueIndex];
	SetCurrentQualityPreset(NextQualityPreset);
}

void USettingRowWidget_QualityPresets::PreviousValue()
{
	if (CurrentSelectedValueIndex <= 1)
	{
		CurrentSelectedValueIndex = AvailableOptions - 1;
	}
	else
	{
		CurrentSelectedValueIndex--;	
	}
	
	int32 NextQualityPreset = AvailableQualityPresets[CurrentSelectedValueIndex];
	SetCurrentQualityPreset(NextQualityPreset);
}

void USettingRowWidget_QualityPresets::SetCurrentQualityPreset(int32 InQualityPreset)
{
	CurrentQualityPreset = InQualityPreset;
	UpdateSettingValue();
	
	OnValueChanged.Broadcast(SettingInfo.SettingType, CurrentAppliedValueIndex != CurrentSelectedValueIndex);
}

