// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "UI/Widget/Subwidget/SettingRowWidget_MasterVolume.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetMathLibrary.h"

void USettingRowWidget_MasterVolume::SetSelectedValue(float InValue)
{
	CurrentSelectedMasterVolume = UKismetMathLibrary::FCeil(InValue * 100) / 100.f;

	UpdateSettingValue();

	SetCurrentMasterVolume(InValue);
	
}

void USettingRowWidget_MasterVolume::ReceiveInitialValue(float InValue)
{
	CurrentMasterVolume = InValue;
	CurrentSelectedMasterVolume = InValue;
	CurrentAppliedMasterVolume = InValue;
	Slider_Value->SetValue(InValue);
}

void USettingRowWidget_MasterVolume::UpdateSettingValue(bool bOverrideValue)
{
	int32 RoundedValue = UKismetMathLibrary::Round(CurrentSelectedMasterVolume * 100); 
	Text_SettingValue->SetText(FText::FromString(FString::Printf(TEXT("%i%s"), RoundedValue, TEXT("%"))));
}
void USettingRowWidget_MasterVolume::SetCurrentMasterVolume(float InMasterVolume)
{
	CurrentMasterVolume = InMasterVolume;
	CurrentSelectedMasterVolume = CurrentMasterVolume;
	UpdateSettingValue();
	
	OnValueChanged.Broadcast(SettingInfo.SettingType, !FMath::IsNearlyEqual(CurrentSelectedMasterVolume, CurrentAppliedMasterVolume,0.005f));
}

void USettingRowWidget_MasterVolume::ApplySetting()
{
	CurrentAppliedMasterVolume = CurrentSelectedMasterVolume;
	OnValueApplied.Broadcast();
}

void USettingRowWidget_MasterVolume::ResetToDefault()
{
	CurrentSelectedMasterVolume = CurrentAppliedMasterVolume;
	UpdateSettingValue(true);
}
