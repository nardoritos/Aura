// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "UI/Widget/Subwidget/SettingRowWidget_FrameRate.h"

#include "Aura/AuraLogChannels.h"
#include "Components/TextBlock.h"

void USettingRowWidget_FrameRate::ReceiveInitialValue(int32 InValue)
{
	
	CurrentSelectedValueIndex = AvailableFrameRate.Find(InValue);
	SetAppliedValue(CurrentSelectedValueIndex);
	
	SetCurrentFrameRate(InValue);
	AvailableOptions = AvailableFrameRate.Num();

	UE_LOG(LogAura, Log, TEXT("%s has %i options available, with the initial option selected being %i"), *GetClass()->GetName(),AvailableOptions, CurrentSelectedValueIndex)

}

#define LOCTEXT_NAMESPACE "UMG"

void USettingRowWidget_FrameRate::UpdateSettingValue(bool bOverrideValue)
{
	Super::UpdateSettingValue(bOverrideValue);

	if (bOverrideValue)
	{
		CurrentFrameRate = AvailableFrameRate[CurrentSelectedValueIndex];
	}
	FText FrameRateText = FText::AsNumber(CurrentFrameRate);
	
	Text_SettingValue->SetText(FrameRateText);
}

#undef LOCTEXT_NAMESPACE



void USettingRowWidget_FrameRate::NextValue()
{
	Super::NextValue();
	
	int32 NextFrameRate = AvailableFrameRate[CurrentSelectedValueIndex];
	SetCurrentFrameRate(NextFrameRate);
}

void USettingRowWidget_FrameRate::PreviousValue()
{
	Super::PreviousValue();
	
	int32 NextFrameRate = AvailableFrameRate[CurrentSelectedValueIndex];
	SetCurrentFrameRate(NextFrameRate);
}

void USettingRowWidget_FrameRate::SetCurrentFrameRate(int32 InFrameRate)
{
	CurrentFrameRate = InFrameRate;
	UpdateSettingValue();
	
	OnValueChanged.Broadcast(SettingInfo.SettingType, CurrentAppliedValueIndex != CurrentSelectedValueIndex);
}

