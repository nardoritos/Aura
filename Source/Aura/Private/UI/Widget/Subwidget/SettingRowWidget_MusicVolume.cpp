// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "UI/Widget/Subwidget/SettingRowWidget_MusicVolume.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetMathLibrary.h"

void USettingRowWidget_MusicVolume::SetSelectedValue(float InValue)
{
	CurrentSelectedMusicVolume = UKismetMathLibrary::FCeil(InValue * 100) / 100.f;

	UpdateSettingValue();

	SetCurrentMusicVolume(InValue);
	
}

void USettingRowWidget_MusicVolume::ReceiveInitialValue(float InValue)
{
	CurrentMusicVolume = InValue;
	CurrentSelectedMusicVolume = InValue;
	CurrentAppliedMusicVolume = InValue;
	Slider_Value->SetValue(InValue);
}

void USettingRowWidget_MusicVolume::UpdateSettingValue(bool bOverrideValue)
{
	int32 RoundedValue = UKismetMathLibrary::Round(CurrentSelectedMusicVolume * 100); 
	Text_SettingValue->SetText(FText::FromString(FString::Printf(TEXT("%i%s"), RoundedValue, TEXT("%"))));
}
void USettingRowWidget_MusicVolume::SetCurrentMusicVolume(float InMusicVolume)
{
	CurrentMusicVolume = InMusicVolume;
	CurrentSelectedMusicVolume = CurrentMusicVolume;
	UpdateSettingValue();
	
	OnValueChanged.Broadcast(SettingInfo.SettingType, !FMath::IsNearlyEqual(CurrentSelectedMusicVolume, CurrentAppliedMusicVolume,0.005f));
}

void USettingRowWidget_MusicVolume::ApplySetting()
{
	CurrentAppliedMusicVolume = CurrentSelectedMusicVolume;
	OnValueApplied.Broadcast();
}

void USettingRowWidget_MusicVolume::ResetToDefault()
{
	CurrentSelectedMusicVolume = CurrentAppliedMusicVolume;
	UpdateSettingValue(true);
}
