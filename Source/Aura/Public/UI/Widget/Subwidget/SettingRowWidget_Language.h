// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SettingRowWidget.h"
#include "SettingRowWidget_Language.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API USettingRowWidget_Language : public USettingRowWidget
{
	GENERATED_BODY()
	
public:
	
	void ReceiveInitialValue(FString InValue);
	
	virtual void UpdateSettingValue(bool bOverrideValue = false) override;

	virtual void NextValue() override;
	virtual void PreviousValue() override;

	void SetCurrentLanguage(FString InLanguage);
	FString GetCurrentLanguage() const { return CurrentLanguage; }

protected:
	
	UPROPERTY()
	FString CurrentLanguage;

	// @param Value 0:en, 1:pt
	UPROPERTY()
	TArray<FString> AvailableLanguageOptions = { "en", "pt"};
};