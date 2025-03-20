// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "UI/Widget/Subwidget/SettingRowWidget_Language.h"

#include "Aura/AuraLogChannels.h"
#include "Components/TextBlock.h"

void USettingRowWidget_Language::ReceiveInitialValue(FString InValue)
{
	CurrentSelectedValueIndex = AvailableLanguageOptions.Find(InValue);
	SetAppliedValue(CurrentSelectedValueIndex);
	
	SetCurrentLanguage(InValue);
	AvailableOptions = AvailableLanguageOptions.Num();

	UE_LOG(LogAura, Log, TEXT("%s has %i options available, with the initial option selected being %i"), *GetClass()->GetName(),AvailableOptions, CurrentSelectedValueIndex)
}


#define LOCTEXT_NAMESPACE "UMG"

void USettingRowWidget_Language::UpdateSettingValue(bool bOverrideValue)
{
	Super::UpdateSettingValue(bOverrideValue);

	if (bOverrideValue)
	{
		CurrentLanguage = AvailableLanguageOptions[CurrentSelectedValueIndex];
	}
	FText LanguageText;
	
	switch (CurrentSelectedValueIndex){
	case 0:
		LanguageText = FText(LOCTEXT("Settings.Language.English", "English"));
		break;
	case 1:
		LanguageText = FText(LOCTEXT("Settings.Language.Portuguese", "Portuguese"));
		break;
	default:
		LanguageText = FText::FromString("Undefined");
		break;
	}
	Text_SettingValue->SetText(LanguageText);
}

#undef LOCTEXT_NAMESPACE



void USettingRowWidget_Language::NextValue()
{
	Super::NextValue();
	
	FString NextLanguage = AvailableLanguageOptions[CurrentSelectedValueIndex];
	SetCurrentLanguage(NextLanguage);
}

void USettingRowWidget_Language::PreviousValue()
{
	Super::PreviousValue();
	
	FString NextLanguage = AvailableLanguageOptions[CurrentSelectedValueIndex];
	SetCurrentLanguage(NextLanguage);
}

void USettingRowWidget_Language::SetCurrentLanguage(FString InLanguage)
{
	CurrentLanguage = InLanguage;
	UpdateSettingValue();
	
	OnValueChanged.Broadcast(SettingInfo.SettingType, CurrentAppliedValueIndex != CurrentSelectedValueIndex);
}

