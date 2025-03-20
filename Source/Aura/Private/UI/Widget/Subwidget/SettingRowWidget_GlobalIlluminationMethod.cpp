// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "UI/Widget/Subwidget/SettingRowWidget_GlobalIlluminationMethod.h"

#include "Aura/AuraLogChannels.h"
#include "Components/TextBlock.h"

void USettingRowWidget_GlobalIlluminationMethod::ReceiveInitialValue(int32 InValue)
{
	CurrentSelectedValueIndex = AvailableGlobalIlluminationMethodOptions.Find(InValue);
	SetAppliedValue(CurrentSelectedValueIndex);
	
	SetCurrentGlobalIlluminationMethod(InValue);
	AvailableOptions = AvailableGlobalIlluminationMethodOptions.Num();

	UE_LOG(LogAura, Log, TEXT("%s has %i options available, with the initial option selected being %i"), *GetClass()->GetName(),AvailableOptions, CurrentSelectedValueIndex)
}


#define LOCTEXT_NAMESPACE "UMG"

void USettingRowWidget_GlobalIlluminationMethod::UpdateSettingValue(bool bOverrideValue)
{
	Super::UpdateSettingValue(bOverrideValue);

	if (bOverrideValue)
	{
		CurrentGlobalIlluminationMethod = AvailableGlobalIlluminationMethodOptions[CurrentSelectedValueIndex];
	}
	FText GlobalIlluminationMethodText;

	switch (CurrentGlobalIlluminationMethod)
	{
	case 0:
		GlobalIlluminationMethodText = FText(LOCTEXT("Settings.GlobalIlluminationMethod.Disabled", "Disabled"));
		break;
	case 1:
		GlobalIlluminationMethodText = FText(LOCTEXT("Settings.GlobalIlluminationMethod.Lumen", "Lumen"));
		break;
	case 2:
		GlobalIlluminationMethodText = FText(LOCTEXT("Settings.GlobalIlluminationMethod.ScreenSpace", "Screen Space"));
		break;
	default:
		GlobalIlluminationMethodText = FText::FromString("Undefined");
		break;
	}
	Text_SettingValue->SetText(GlobalIlluminationMethodText);
}

#undef LOCTEXT_NAMESPACE



void USettingRowWidget_GlobalIlluminationMethod::NextValue()
{
	Super::NextValue();
	
	int32 NextGlobalIlluminationMethod = AvailableGlobalIlluminationMethodOptions[CurrentSelectedValueIndex];
	SetCurrentGlobalIlluminationMethod(NextGlobalIlluminationMethod);
}

void USettingRowWidget_GlobalIlluminationMethod::PreviousValue()
{
	Super::PreviousValue();
	
	int32 NextGlobalIlluminationMethod = AvailableGlobalIlluminationMethodOptions[CurrentSelectedValueIndex];
	SetCurrentGlobalIlluminationMethod(NextGlobalIlluminationMethod);
}

void USettingRowWidget_GlobalIlluminationMethod::SetCurrentGlobalIlluminationMethod(int32 InGlobalIlluminationMethod)
{
	CurrentGlobalIlluminationMethod = InGlobalIlluminationMethod;
	UpdateSettingValue();
	
	OnValueChanged.Broadcast(SettingInfo.SettingType, CurrentAppliedValueIndex != CurrentSelectedValueIndex);
}

