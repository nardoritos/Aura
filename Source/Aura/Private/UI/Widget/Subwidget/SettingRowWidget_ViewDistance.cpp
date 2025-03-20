// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "UI/Widget/Subwidget/SettingRowWidget_ViewDistance.h"

#include "Aura/AuraLogChannels.h"
#include "Components/TextBlock.h"

void USettingRowWidget_ViewDistance::ReceiveInitialValue(int32 InValue)
{
	CurrentSelectedValueIndex = AvailableViewDistance.Find(InValue);
	SetAppliedValue(CurrentSelectedValueIndex);
	
	SetCurrentViewDistance(InValue);
	AvailableOptions = AvailableViewDistance.Num();

	UE_LOG(LogAura, Log, TEXT("SettingRowWidget_ViewDistance has %i options available, with the initial option selected being %i") ,AvailableOptions, CurrentSelectedValueIndex)

}

#define LOCTEXT_NAMESPACE "UMG"

void USettingRowWidget_ViewDistance::UpdateSettingValue(bool bOverrideValue)
{
	Super::UpdateSettingValue(bOverrideValue);

	if (bOverrideValue)
	{
		CurrentViewDistance = AvailableViewDistance[CurrentSelectedValueIndex];
	}
	FText ViewDistanceText;

	switch (CurrentViewDistance)
	{
	case 0:
		ViewDistanceText = FText(LOCTEXT("Settings.ViewDistance.Low", "Near"));
		break;
	case 1:
		ViewDistanceText = FText(LOCTEXT("Settings.ViewDistance.Medium", "Medium"));
		break;
	case 2:
		ViewDistanceText = FText(LOCTEXT("Settings.ViewDistance.High", "Far"));
		break;
	case 3:
		ViewDistanceText = FText(LOCTEXT("Settings.ViewDistance.Epic", "Epic"));
		break;
	case 4:
		ViewDistanceText = FText(LOCTEXT("Settings.ViewDistance.Cinematic", "Cinematic"));
		break;
	default:
		ViewDistanceText = FText::FromString("Undefined");
		break;
	}
	Text_SettingValue->SetText(ViewDistanceText);
}

#undef LOCTEXT_NAMESPACE



void USettingRowWidget_ViewDistance::NextValue()
{
	Super::NextValue();
	
	int32 NextViewDistance = AvailableViewDistance[CurrentSelectedValueIndex];
	SetCurrentViewDistance(NextViewDistance);
}

void USettingRowWidget_ViewDistance::PreviousValue()
{
	Super::PreviousValue();
	
	int32 NextViewDistance = AvailableViewDistance[CurrentSelectedValueIndex];
	SetCurrentViewDistance(NextViewDistance);
}

void USettingRowWidget_ViewDistance::SetCurrentViewDistance(int32 InViewDistance)
{
	CurrentViewDistance = InViewDistance;
	UpdateSettingValue();
	
	OnValueChanged.Broadcast(SettingInfo.SettingType, CurrentAppliedValueIndex != CurrentSelectedValueIndex);
}

