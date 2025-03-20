// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingWidget.generated.h"

class USettingRowWidget;
class UVerticalBox;
class USettingsInfo;

UENUM(BlueprintType)
enum ESettingType
{
	// Default
	NoSetting,
	
	// Video
	WindowMode,
	Resolution,
	QualityPresets,
	GlobalIlluminationMethod,
	GlobalIlluminationQuality,
	ShadowsQuality,
	AntiAliasingMethod,
	AntiAliasingQuality,
	ViewDistance,
	TexturesQuality,
	EffectsQuality,
	ReflectionsQuality,
	PostProcessingQuality,
	VSync,
	FrameRate,

	// Audio
	MasterVolume,
	MusicVolume,
	SoundEffects,

	// Language
	Language
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSettingChangedValue, ESettingType, SettingType, bool, bShouldEnableButtons);

UCLASS()
class AURA_API USettingWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateButtonsState(bool bShouldEnableButtons);

	UFUNCTION(BlueprintCallable)
	void ApplySettings();
	
	UFUNCTION(BlueprintCallable)
	void ResetToDefaults();
	
	UPROPERTY()
	TMap<TEnumAsByte<ESettingType>, USettingRowWidget*> SettingsMap; 
	
	TMap<TEnumAsByte<ESettingType>, bool> SettingsDirtyState;

	void OnSettingValueChanged(ESettingType SettingType, bool bIsDifferentThanCurrentlyAppliedValue);

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UVerticalBox> Box_SettingsList;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USettingsInfo> SettingsInfo;
};
