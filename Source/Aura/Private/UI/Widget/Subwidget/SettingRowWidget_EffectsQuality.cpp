// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "UI/Widget/Subwidget/SettingRowWidget_EffectsQuality.h"

#include "Aura/AuraLogChannels.h"
#include "Components/TextBlock.h"

void USettingRowWidget_EffectsQuality::ReceiveInitialValue(int32 InValue)
{
	CurrentSelectedValueIndex = AvailableEffectsQuality.Find(InValue);
	SetAppliedValue(CurrentSelectedValueIndex);
	
	SetCurrentEffectsQuality(InValue);
	AvailableOptions = AvailableEffectsQuality.Num();

	UE_LOG(LogAura, Log, TEXT("%s has %i options available, with the initial option selected being %i"), *GetClass()->GetName(),AvailableOptions, CurrentSelectedValueIndex)

}
#define LOCTEXT_NAMESPACE "UMG"

void USettingRowWidget_EffectsQuality::UpdateSettingValue(bool bOverrideValue)
{
	Super::UpdateSettingValue(bOverrideValue);

	if (bOverrideValue)
	{
		CurrentEffectsQuality = AvailableEffectsQuality[CurrentSelectedValueIndex];
	}
	FText EffectsQualityText;

	switch (CurrentEffectsQuality)
	{
	case 0:
		EffectsQualityText = FText(LOCTEXT("Settings.EffectsQuality.Low", "Low"));
		break;
	case 1:
		EffectsQualityText = FText(LOCTEXT("Settings.EffectsQuality.Medium", "Medium"));
		break;
	case 2:
		EffectsQualityText = FText(LOCTEXT("Settings.EffectsQuality.High", "High"));
		break;
	case 3:
		EffectsQualityText = FText(LOCTEXT("Settings.EffectsQuality.Epic", "Epic"));
		break;
	case 4:
		EffectsQualityText = FText(LOCTEXT("Settings.EffectsQuality.Cinematic", "Cinematic"));
		break;
	default:
		EffectsQualityText = FText::FromString("Undefined");
		break;
	}
	Text_SettingValue->SetText(EffectsQualityText);
}

#undef LOCTEXT_NAMESPACE


void USettingRowWidget_EffectsQuality::NextValue()
{
	Super::NextValue();
	
	int32 NextEffectsQuality = AvailableEffectsQuality[CurrentSelectedValueIndex];
	SetCurrentEffectsQuality(NextEffectsQuality);
}

void USettingRowWidget_EffectsQuality::PreviousValue()
{
	Super::PreviousValue();
	
	int32 NextEffectsQuality = AvailableEffectsQuality[CurrentSelectedValueIndex];
	SetCurrentEffectsQuality(NextEffectsQuality);
}

void USettingRowWidget_EffectsQuality::SetCurrentEffectsQuality(int32 InEffectsQuality)
{
	CurrentEffectsQuality = InEffectsQuality;
	UpdateSettingValue();
	
	OnValueChanged.Broadcast(SettingInfo.SettingType, CurrentAppliedValueIndex != CurrentSelectedValueIndex);
}

