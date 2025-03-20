// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "UI/Widget/Subwidget/SettingRowWidget_VSync.h"

#include "Aura/AuraLogChannels.h"
#include "Components/TextBlock.h"

void USettingRowWidget_VSync::ReceiveInitialValue(int32 InValue)
{
	bool bValue = static_cast<bool>(InValue);
	CurrentSelectedValueIndex = AvailableVSync.Find(bValue);
	SetAppliedValue(CurrentSelectedValueIndex);
	
	SetCurrentVSync(bValue);
	AvailableOptions = AvailableVSync.Num();

	UE_LOG(LogAura, Log, TEXT("%s has %i options available, with the initial option selected being %i"), *GetClass()->GetName(),AvailableOptions, CurrentSelectedValueIndex)

}
 
#define LOCTEXT_NAMESPACE "UMG"

void USettingRowWidget_VSync::UpdateSettingValue(bool bOverrideValue)
{
	Super::UpdateSettingValue(bOverrideValue);

	if (bOverrideValue)
	{
		CurrentVSync = AvailableVSync[CurrentSelectedValueIndex];
	}
	FText VSyncText;

	if (CurrentVSync)
	
		VSyncText = FText(LOCTEXT("Settings.VSync.Off", "Off"));
	else
		VSyncText = FText(LOCTEXT("Settings.VSync.On", "On"));
	
	Text_SettingValue->SetText(VSyncText);
}

#undef LOCTEXT_NAMESPACE



void USettingRowWidget_VSync::NextValue()
{
	Super::NextValue();
	
	bool NextVSync = AvailableVSync[CurrentSelectedValueIndex];
	SetCurrentVSync(NextVSync);
}

void USettingRowWidget_VSync::PreviousValue()
{
	Super::PreviousValue();
	
	bool NextVSync = AvailableVSync[CurrentSelectedValueIndex];
	SetCurrentVSync(NextVSync);
}

void USettingRowWidget_VSync::SetCurrentVSync(bool  InVSync)
{
	CurrentVSync = InVSync;
	UpdateSettingValue();
	
	OnValueChanged.Broadcast(SettingInfo.SettingType, CurrentAppliedValueIndex != CurrentSelectedValueIndex);
}

