// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "UI/Widget/Subwidget/SettingRowWidget_Resolution.h"
#include "UMG.h"
#include "Aura/AuraLogChannels.h"

void USettingRowWidget_Resolution::ReceiveInitialValue(FIntPoint InValue)
{
	CurrentSelectedValueIndex = ConvenientResolutions.Find(InValue);
	SetAppliedValue(CurrentSelectedValueIndex);
	
	AvailableOptions = ConvenientResolutions.Num();
	SetCurrentResolution(InValue);
	

	UE_LOG(LogAura, Log, TEXT("SettingRowWidget_Resolution has %i options available, with the initial option selected being %i, %ix%i"), AvailableOptions, CurrentSelectedValueIndex
		, InValue.X, InValue.Y);

	
}

void USettingRowWidget_Resolution::UpdateSettingValue(bool bOverrideValue)
{
	Super::UpdateSettingValue();
	
	if (bOverrideValue)
	{
		CurrentResolution = ConvenientResolutions[CurrentSelectedValueIndex];
	}
	
	FText FormatResolution = FText::FromString(FString::Printf(TEXT("%ix%i"), CurrentResolution.X, CurrentResolution.Y));
	
	Text_SettingValue->SetText(FormatResolution);
}

void USettingRowWidget_Resolution::NextValue()
{
	Super::NextValue();
	
	FIntPoint NextResolution = ConvenientResolutions[CurrentSelectedValueIndex];
	SetCurrentResolution(NextResolution);
}

void USettingRowWidget_Resolution::PreviousValue()
{
	Super::PreviousValue();

	FIntPoint NextResolution = ConvenientResolutions[CurrentSelectedValueIndex];
	SetCurrentResolution(NextResolution);
}

void USettingRowWidget_Resolution::SetCurrentResolution(FIntPoint InResolution)
{
	CurrentResolution = InResolution;
	UpdateSettingValue();

	OnValueChanged.Broadcast(SettingInfo.SettingType, CurrentAppliedValueIndex != CurrentSelectedValueIndex);
}

void USettingRowWidget_Resolution::SetAvailableResolutions(TArray<FIntPoint> InAvailableResolutions)
{
	ConvenientResolutions = InAvailableResolutions;
	AvailableOptions = ConvenientResolutions.Num();
}
