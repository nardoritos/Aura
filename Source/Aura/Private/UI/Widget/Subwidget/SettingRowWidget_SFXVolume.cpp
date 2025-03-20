// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "UI/Widget/Subwidget/SettingRowWidget_SFXVolume.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetMathLibrary.h"

void USettingRowWidget_SFXVolume::SetSelectedValue(float InValue)
{
	CurrentSelectedSFXVolume = UKismetMathLibrary::FCeil(InValue * 100) / 100.f;

	UpdateSettingValue();

	SetCurrentSFXVolume(InValue);
	
}

void USettingRowWidget_SFXVolume::ReceiveInitialValue(float InValue)
{
	CurrentSFXVolume = InValue;
	CurrentSelectedSFXVolume = InValue;
	CurrentAppliedSFXVolume = InValue;
	Slider_Value->SetValue(InValue);
}

void USettingRowWidget_SFXVolume::UpdateSettingValue(bool bOverrideValue)
{
	int32 RoundedValue = UKismetMathLibrary::Round(CurrentSelectedSFXVolume * 100); 
	Text_SettingValue->SetText(FText::FromString(FString::Printf(TEXT("%i%s"), RoundedValue, TEXT("%"))));
}
void USettingRowWidget_SFXVolume::SetCurrentSFXVolume(float InSFXVolume)
{
	CurrentSFXVolume = InSFXVolume;
	CurrentSelectedSFXVolume = CurrentSFXVolume;
	UpdateSettingValue();
	
	OnValueChanged.Broadcast(SettingInfo.SettingType, !FMath::IsNearlyEqual(CurrentSelectedSFXVolume, CurrentAppliedSFXVolume,0.005f));
}

void USettingRowWidget_SFXVolume::ApplySetting()
{
	CurrentAppliedSFXVolume = CurrentSelectedSFXVolume;
	OnValueApplied.Broadcast();
}

void USettingRowWidget_SFXVolume::ResetToDefault()
{
	CurrentSelectedSFXVolume = CurrentAppliedSFXVolume;
	UpdateSettingValue(true);
}
