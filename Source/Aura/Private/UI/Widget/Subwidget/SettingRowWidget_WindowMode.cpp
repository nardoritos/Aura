// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "UI/Widget/Subwidget/SettingRowWidget_WindowMode.h"
#include "UMG.h"
#include "Aura/AuraLogChannels.h"



void USettingRowWidget_WindowMode::ReceiveInitialValue(EWindowMode::Type InWindowMode)
{
	CurrentSelectedValueIndex = WindowModes.Find(InWindowMode);
	SetAppliedValue(CurrentSelectedValueIndex);
	
	SetCurrentWindowMode(InWindowMode);
	AvailableOptions = WindowModes.Num();

	UE_LOG(LogAura, Log, TEXT("SettingRowWidget_WindowMode has %i options available, with the initial option selected being %i") ,AvailableOptions, CurrentSelectedValueIndex)
}


#define LOCTEXT_NAMESPACE "UMG"

void USettingRowWidget_WindowMode::UpdateSettingValue(bool bOverrideValue)
{
	Super::UpdateSettingValue();

	if (bOverrideValue)
	{
		CurrentWindowMode = WindowModes[CurrentSelectedValueIndex];
	}
	FText WindowModeString;

	switch (CurrentWindowMode)
	{
	case EWindowMode::Type::Fullscreen:
		WindowModeString = FText(LOCTEXT("Settings.WindowMode.Fullscreen", "Fullscreen"));
		break;
	case EWindowMode::Type::Windowed:
		WindowModeString = FText(LOCTEXT("Settings.WindowMode.Windowed", "Windowed"));
		break;
	case EWindowMode::Type::WindowedFullscreen:
		WindowModeString = FText(LOCTEXT("Settings.WindowMode.WindowedFullscreen", "Windowed Fullscreen"));
		break;
	default:
		WindowModeString = FText::FromString("Undefined");
		break;
	}
	
	Text_SettingValue->SetText(WindowModeString);
}
#undef LOCTEXT_NAMESPACE


void USettingRowWidget_WindowMode::NextValue()
{
	Super::NextValue();
	
	EWindowMode::Type NextWindowMode = WindowModes[CurrentSelectedValueIndex];
	SetCurrentWindowMode(NextWindowMode);
}

void USettingRowWidget_WindowMode::PreviousValue()
{
	Super::PreviousValue();
	
	EWindowMode::Type NextWindowMode = WindowModes[CurrentSelectedValueIndex];
	SetCurrentWindowMode(NextWindowMode);
}

void USettingRowWidget_WindowMode::SetCurrentWindowMode(EWindowMode::Type InWindowMode)
{
	CurrentWindowMode = InWindowMode;
	UpdateSettingValue();
	
	OnValueChanged.Broadcast(SettingInfo.SettingType, CurrentAppliedValueIndex != CurrentSelectedValueIndex);
}


