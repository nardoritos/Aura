// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "UI/Widget/Subwidget/SettingRowWidget_GlobalIlluminationQuality.h"

#include "Aura/AuraLogChannels.h"
#include "Components/TextBlock.h"

void USettingRowWidget_GlobalIlluminationQuality::ReceiveInitialValue(int32 InValue)
{
	CurrentSelectedValueIndex = AvailableGlobalIlluminationQualityOptions.Find(InValue);
	SetAppliedValue(CurrentSelectedValueIndex);
	
	SetCurrentGlobalIlluminationQuality(InValue);
	AvailableOptions = AvailableGlobalIlluminationQualityOptions.Num();

	UE_LOG(LogAura, Log, TEXT("%s has %i options available, with the initial option selected being %i"), *GetClass()->GetName(),AvailableOptions, CurrentSelectedValueIndex)
}


#define LOCTEXT_NAMESPACE "UMG"

void USettingRowWidget_GlobalIlluminationQuality::UpdateSettingValue(bool bOverrideValue)
{
	Super::UpdateSettingValue(bOverrideValue);

	if (bOverrideValue)
	{
		CurrentGlobalIlluminationQuality = AvailableGlobalIlluminationQualityOptions[CurrentSelectedValueIndex];
	}
	FText GlobalIlluminationQualityText;

	switch (CurrentGlobalIlluminationQuality)
	{
	case -1:
		GlobalIlluminationQualityText = FText(LOCTEXT("Settings.GlobalIlluminationQuality.Disabled", "Disabled"));
		break;
	case 0:
		GlobalIlluminationQualityText = FText(LOCTEXT("Settings.GlobalIlluminationQuality.Low", "Low"));
		break;
	case 1:
		GlobalIlluminationQualityText = FText(LOCTEXT("Settings.GlobalIlluminationQuality.Medium", "Medium"));
		break;
	case 2:
		GlobalIlluminationQualityText = FText(LOCTEXT("Settings.GlobalIlluminationQuality.High", "High"));
		break;
	case 3:
		GlobalIlluminationQualityText = FText(LOCTEXT("Settings.GlobalIlluminationQuality.Epic", "Epic"));
		break;
	case 4:
		GlobalIlluminationQualityText = FText(LOCTEXT("Settings.GlobalIlluminationQuality.Cinematic", "Cinematic"));
		break;
	default:
		GlobalIlluminationQualityText = FText::FromString("Undefined");
		break;
	}
	Text_SettingValue->SetText(GlobalIlluminationQualityText);
}

#undef LOCTEXT_NAMESPACE



void USettingRowWidget_GlobalIlluminationQuality::NextValue()
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
	
	int32 NextGlobalIlluminationQuality = AvailableGlobalIlluminationQualityOptions[CurrentSelectedValueIndex];
	SetCurrentGlobalIlluminationQuality(NextGlobalIlluminationQuality);
}

void USettingRowWidget_GlobalIlluminationQuality::PreviousValue()
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
	
	int32 NextGlobalIlluminationQuality = AvailableGlobalIlluminationQualityOptions[CurrentSelectedValueIndex];
	SetCurrentGlobalIlluminationQuality(NextGlobalIlluminationQuality);
}

void USettingRowWidget_GlobalIlluminationQuality::SetCurrentGlobalIlluminationQuality(int32 InGlobalIlluminationQuality)
{
	CurrentGlobalIlluminationQuality = InGlobalIlluminationQuality;
	UpdateSettingValue();
	
	OnValueChanged.Broadcast(SettingInfo.SettingType, CurrentAppliedValueIndex != CurrentSelectedValueIndex);
}

