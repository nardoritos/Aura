// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "UI/Widget/Subwidget/SettingRowWidget_PostProcessingQuality.h"

#include "Aura/AuraLogChannels.h"
#include "Components/TextBlock.h"

void USettingRowWidget_PostProcessingQuality::ReceiveInitialValue(int32 InValue)
{
	CurrentSelectedValueIndex = AvailablePostProcessingQuality.Find(InValue);
	SetAppliedValue(CurrentSelectedValueIndex);
	
	SetCurrentPostProcessingQuality(InValue);
	AvailableOptions = AvailablePostProcessingQuality.Num();

	UE_LOG(LogAura, Log, TEXT("%s has %i options available, with the initial option selected being %i"), *GetClass()->GetName(),AvailableOptions, CurrentSelectedValueIndex)

}

#define LOCTEXT_NAMESPACE "UMG"

void USettingRowWidget_PostProcessingQuality::UpdateSettingValue(bool bOverrideValue)
{
	Super::UpdateSettingValue(bOverrideValue);

	if (bOverrideValue)
	{
		CurrentPostProcessingQuality = AvailablePostProcessingQuality[CurrentSelectedValueIndex];
	}
	FText PostProcessingQualityText;

	switch (CurrentPostProcessingQuality)
	{
	case 0:
		PostProcessingQualityText = FText(LOCTEXT("Settings.PostProcessingQuality.Low", "Low"));
		break;
	case 1:
		PostProcessingQualityText = FText(LOCTEXT("Settings.PostProcessingQuality.Medium", "Medium"));
		break;
	case 2:
		PostProcessingQualityText = FText(LOCTEXT("Settings.PostProcessingQuality.High", "High"));
		break;
	case 3:
		PostProcessingQualityText = FText(LOCTEXT("Settings.PostProcessingQuality.Epic", "Epic"));
		break;
	case 4:
		PostProcessingQualityText = FText(LOCTEXT("Settings.PostProcessingQuality.Cinematic", "Cinematic"));
		break;
	default:
		PostProcessingQualityText = FText::FromString("Undefined");
		break;
	}
	Text_SettingValue->SetText(PostProcessingQualityText);
}

#undef LOCTEXT_NAMESPACE



void USettingRowWidget_PostProcessingQuality::NextValue()
{
	Super::NextValue();
	
	int32 NextPostProcessingQuality = AvailablePostProcessingQuality[CurrentSelectedValueIndex];
	SetCurrentPostProcessingQuality(NextPostProcessingQuality);
}

void USettingRowWidget_PostProcessingQuality::PreviousValue()
{
	Super::PreviousValue();
	
	int32 NextPostProcessingQuality = AvailablePostProcessingQuality[CurrentSelectedValueIndex];
	SetCurrentPostProcessingQuality(NextPostProcessingQuality);
}

void USettingRowWidget_PostProcessingQuality::SetCurrentPostProcessingQuality(int32 InPostProcessingQuality)
{
	CurrentPostProcessingQuality = InPostProcessingQuality;
	UpdateSettingValue();
	
	OnValueChanged.Broadcast(SettingInfo.SettingType, CurrentAppliedValueIndex != CurrentSelectedValueIndex);
}

