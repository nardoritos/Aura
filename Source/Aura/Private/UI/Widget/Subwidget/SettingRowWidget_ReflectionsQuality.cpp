// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "UI/Widget/Subwidget/SettingRowWidget_ReflectionsQuality.h"

#include "Aura/AuraLogChannels.h"
#include "Components/TextBlock.h"

void USettingRowWidget_ReflectionsQuality::ReceiveInitialValue(int32 InValue)
{
	CurrentSelectedValueIndex = AvailableReflectionsQuality.Find(InValue);
	SetAppliedValue(CurrentSelectedValueIndex);
	
	SetCurrentReflectionsQuality(InValue);
	AvailableOptions = AvailableReflectionsQuality.Num();

	UE_LOG(LogAura, Log, TEXT("%s has %i options available, with the initial option selected being %i"), *GetClass()->GetName(),AvailableOptions, CurrentSelectedValueIndex)

}

#define LOCTEXT_NAMESPACE "UMG"

void USettingRowWidget_ReflectionsQuality::UpdateSettingValue(bool bOverrideValue)
{
	Super::UpdateSettingValue(bOverrideValue);

	if (bOverrideValue)
	{
		CurrentReflectionsQuality = AvailableReflectionsQuality[CurrentSelectedValueIndex];
	}
	FText ReflectionsQualityText;

	switch (CurrentReflectionsQuality)
	{
	case 0:
		ReflectionsQualityText = FText(LOCTEXT("Settings.ReflectionsQuality.Low", "Low"));
		break;
	case 1:
		ReflectionsQualityText = FText(LOCTEXT("Settings.ReflectionsQuality.Medium", "Medium"));
		break;
	case 2:
		ReflectionsQualityText = FText(LOCTEXT("Settings.ReflectionsQuality.High", "High"));
		break;
	case 3:
		ReflectionsQualityText = FText(LOCTEXT("Settings.ReflectionsQuality.Epic", "Epic"));
		break;
	case 4:
		ReflectionsQualityText = FText(LOCTEXT("Settings.ReflectionsQuality.Cinematic", "Cinematic"));
		break;
	default:
		ReflectionsQualityText = FText::FromString("Undefined");
		break;
	}
	Text_SettingValue->SetText(ReflectionsQualityText);
}

#undef LOCTEXT_NAMESPACE



void USettingRowWidget_ReflectionsQuality::NextValue()
{
	Super::NextValue();
	
	int32 NextReflectionsQuality = AvailableReflectionsQuality[CurrentSelectedValueIndex];
	SetCurrentReflectionsQuality(NextReflectionsQuality);
}

void USettingRowWidget_ReflectionsQuality::PreviousValue()
{
	Super::PreviousValue();
	
	int32 NextReflectionsQuality = AvailableReflectionsQuality[CurrentSelectedValueIndex];
	SetCurrentReflectionsQuality(NextReflectionsQuality);
}

void USettingRowWidget_ReflectionsQuality::SetCurrentReflectionsQuality(int32 InReflectionsQuality)
{
	CurrentReflectionsQuality = InReflectionsQuality;
	UpdateSettingValue();
	
	OnValueChanged.Broadcast(SettingInfo.SettingType, CurrentAppliedValueIndex != CurrentSelectedValueIndex);
}

