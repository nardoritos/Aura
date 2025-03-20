// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "UI/Widget/Subwidget/SettingRowWidget_AAQuality.h"

#include "Aura/AuraLogChannels.h"
#include "Components/TextBlock.h"

void USettingRowWidget_AAQuality::ReceiveInitialValue(int32 InValue)
{
	CurrentSelectedValueIndex = AvailableAAQualityOptions.Find(InValue);
	SetAppliedValue(CurrentSelectedValueIndex);
	
	SetCurrentAAQuality(InValue);
	AvailableOptions = AvailableAAQualityOptions.Num();

	UE_LOG(LogAura, Log, TEXT("%s has %i options available, with the initial option selected being %i"), *GetClass()->GetName(),AvailableOptions, CurrentSelectedValueIndex)
}


#define LOCTEXT_NAMESPACE "UMG"

void USettingRowWidget_AAQuality::UpdateSettingValue(bool bOverrideValue)
{
	Super::UpdateSettingValue(bOverrideValue);

	if (bOverrideValue)
	{
		CurrentAAQuality = AvailableAAQualityOptions[CurrentSelectedValueIndex];
	}
	FText AAQualityText;

	switch (CurrentAAQuality)
	{
	case -1:
		AAQualityText = FText(LOCTEXT("Settings.AAQuality.Disabled", "Disabled"));
		break;
	case 0:
		AAQualityText = FText(LOCTEXT("Settings.AAQuality.Low", "Low"));
		break;
	case 1:
		AAQualityText = FText(LOCTEXT("Settings.AAQuality.Medium", "Medium"));
		break;
	case 2:
		AAQualityText = FText(LOCTEXT("Settings.AAQuality.High", "High"));
		break;
	case 3:
		AAQualityText = FText(LOCTEXT("Settings.AAQuality.Epic", "Epic"));
		break;
	case 4:
		AAQualityText = FText(LOCTEXT("Settings.AAQuality.Cinematic", "Cinematic"));
		break;
	default:
		AAQualityText = FText::FromString("Undefined");
		break;
	}
	Text_SettingValue->SetText(AAQualityText);
}

#undef LOCTEXT_NAMESPACE



void USettingRowWidget_AAQuality::NextValue()
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
	
	int32 NextAAQuality = AvailableAAQualityOptions[CurrentSelectedValueIndex];
	SetCurrentAAQuality(NextAAQuality);
}

void USettingRowWidget_AAQuality::PreviousValue()
{
	if (CurrentSelectedValueIndex <= 1)
	{
		CurrentSelectedValueIndex = AvailableOptions - 1;
	}
	else
	{
		CurrentSelectedValueIndex--;	
	}
	UpdateSettingValue();
	
	int32 NextAAQuality = AvailableAAQualityOptions[CurrentSelectedValueIndex];
	SetCurrentAAQuality(NextAAQuality);
}

void USettingRowWidget_AAQuality::SetCurrentAAQuality(int32 InAAQuality)
{
	CurrentAAQuality = InAAQuality;
	UpdateSettingValue();
	
	OnValueChanged.Broadcast(SettingInfo.SettingType, CurrentAppliedValueIndex != CurrentSelectedValueIndex);
}

