// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "UI/Widget/SettingWidget.h"
#include "AuraGameUserSettings.generated.h"

class USettingRowWidget;
class USettingRowWidget_WindowMode;
class USettingRowWidget_Resolution;
class USettingRowWidget_GlobalIlluminationQuality;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCustomAudioSettingChanged, float /* SettingVolume */);

/**
 * 
 */
UCLASS(Config = AuraGameUserSettings)
class AURA_API UAuraGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:

	virtual void LoadSettings(bool bForceReload = false) override;
	
	void RegisterNewSetting(ESettingType NewSetting, USettingRowWidget* SettingRowWidget);

	USettingRowWidget* GetSettingInSlot(ESettingType SettingToFind);
	
private:
	// Internal Registers
	void Register_WindowMode(USettingRowWidget* SettingRowWidget);
	void Register_Resolution(USettingRowWidget* SettingRowWidget);
	void Register_QualityPresets(USettingRowWidget* SettingRowWidget);
	void Register_GlobalIlluminationMethod(USettingRowWidget* SettingRowWidget);
	void Register_GlobalIlluminationQuality(USettingRowWidget* SettingRowWidget);
	void Register_Shadows(USettingRowWidget* SettingRowWidget);
	void Register_ViewDistance(USettingRowWidget* SettingRowWidget);
	void Register_TextureQuality(USettingRowWidget* SettingRowWidget);
	void Register_EffectsQuality(USettingRowWidget* SettingRowWidget);
	void Register_ReflectionsQuality(USettingRowWidget* SettingRowWidget);
	void Register_PostProcessingQuality(USettingRowWidget* SettingRowWidget);
	void Register_AAMethod(USettingRowWidget* SettingRowWidget);
	void Register_AAQuality(USettingRowWidget* SettingRowWidget);
	void Register_VSync(USettingRowWidget* SettingRowWidget);
	void Register_FrameRate(USettingRowWidget* SettingRowWidget);
	void Register_MasterVolume(USettingRowWidget* SettingRowWidget);
	void Register_SFXVolume(USettingRowWidget* SettingRowWidget);
	void Register_MusicVolume(USettingRowWidget* SettingRowWidget);
	void Register_Language(USettingRowWidget* SettingRowWidget);
	
	// Internal Updaters (For settings that have to be disabled depending on other setting)
	void Update_Resolution(const bool bOverrideResolution = false);
	void Update_GlobalIlluminationQuality(const bool bOverrideGlobalIlluminationQuality = false);
	void Update_AAQuality(const bool bOverrideAAQuality = false);
	void Update_FrameRate(const bool bOverrideFrameRate = false);
	
public:
	/** Utils (that doesn't already exist easily somewhere else) */
	// Global Illumination Method
 	int32 GetGlobalIlluminationMethod();
	void SetGlobalIlluminationMethod(const int32 InMethod);
	// Anti Aliasing Method
	int32 GetAntiAliasingMethod();
	void SetAntiAliasingMethod(const int32 InMethod);
	// Master Volume
	float GetMasterVolume() const { return ConfigMasterVolume; }
	void SetMasterVolume(const float InMasterVolume);
	// SFX Volume
	float GetSFXVolume() const { return ConfigSFXVolume; }
	void SetSFXVolume(const float InSFXVolume);
	// Music Volume
	float GetMusicVolume() const { return ConfigMusicVolume; }
	void SetMusicVolume(const float InMusicVolume);

	FOnCustomAudioSettingChanged OnMasterVolumeApplied;
	FOnCustomAudioSettingChanged OnSFXVolumeApplied;
	FOnCustomAudioSettingChanged OnMusicVolumeApplied;

private:
	
	UPROPERTY(Config)
	int32 ConfigGlobalIlluminationMethod = 2;

	UPROPERTY(Config)
	int32 ConfigAntiAliasingMethod = 1;

	UPROPERTY(Config)
	float ConfigMasterVolume = 0.5f;

	UPROPERTY(Config)
	float ConfigSFXVolume = 1.0f;

	UPROPERTY(Config)
	float ConfigMusicVolume = 1.0f;
	
	UPROPERTY()
	TMap<TEnumAsByte<ESettingType>, USettingRowWidget*> SettingWidgets;
		
};
