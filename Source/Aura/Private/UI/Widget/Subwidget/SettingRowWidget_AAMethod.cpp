// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "UI/Widget/Subwidget/SettingRowWidget_AAMethod.h"

#include "Aura/AuraLogChannels.h"
#include "Components/TextBlock.h"

void USettingRowWidget_AAMethod::ReceiveInitialValue(int32 InValue)
{
	CurrentSelectedValueIndex = AvailableAAMethodOptions.Find(InValue);
	SetAppliedValue(CurrentSelectedValueIndex);
	
	SetCurrentAAMethod(InValue);
	AvailableOptions = AvailableAAMethodOptions.Num();

	UE_LOG(LogAura, Log, TEXT("%s has %i options available, with the initial option selected being %i"), *GetClass()->GetName(),AvailableOptions, CurrentSelectedValueIndex)
}


#define LOCTEXT_NAMESPACE "UMG"

void USettingRowWidget_AAMethod::UpdateSettingValue(bool bOverrideValue)
{
	Super::UpdateSettingValue(bOverrideValue);

	if (bOverrideValue)
	{
		CurrentAAMethod = AvailableAAMethodOptions[CurrentSelectedValueIndex];
	}
	FText AAMethodText;

	switch (CurrentAAMethod)
	{
	case 0:
		AAMethodText = FText(LOCTEXT("Settings.AAMethod.None", "None"));
		break;
	case 1:
		AAMethodText = FText(LOCTEXT("Settings.AAMethod.FXAA", "FXAA"));
		break;
	case 2:
		AAMethodText = FText(LOCTEXT("Settings.AAMethod.TAA", "TAA"));
		break;
	case 3:
		AAMethodText = FText(LOCTEXT("Settings.AAMethod.MSAA", "MSAA"));
		break;
	case 4:
		AAMethodText = FText(LOCTEXT("Settings.AAMethod.TSR", "TSR"));
		break;
	default:
		AAMethodText = FText::FromString("Undefined");
		break;
	}
	Text_SettingValue->SetText(AAMethodText);
}

#undef LOCTEXT_NAMESPACE



void USettingRowWidget_AAMethod::NextValue()
{
	Super::NextValue();
	
	int32 NextAAMethod = AvailableAAMethodOptions[CurrentSelectedValueIndex];
	SetCurrentAAMethod(NextAAMethod);
}

void USettingRowWidget_AAMethod::PreviousValue()
{
	Super::PreviousValue();
	
	int32 NextAAMethod = AvailableAAMethodOptions[CurrentSelectedValueIndex];
	SetCurrentAAMethod(NextAAMethod);
}

void USettingRowWidget_AAMethod::SetCurrentAAMethod(int32 InAAMethod)
{
	CurrentAAMethod = InAAMethod;
	UpdateSettingValue();
	
	OnValueChanged.Broadcast(SettingInfo.SettingType, CurrentAppliedValueIndex != CurrentSelectedValueIndex);
}

