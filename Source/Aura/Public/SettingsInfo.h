// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UI/Widget/SettingWidget.h"
#include "SettingsInfo.generated.h"

class USettingRowWidget;

USTRUCT(BlueprintType)
struct FAuraSettingsInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TEnumAsByte<ESettingType> SettingType = NoSetting;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText SettingName = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<USettingRowWidget> SettingClass;
	
};

UCLASS()
class AURA_API USettingsInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FAuraSettingsInfo> Settings;
};
