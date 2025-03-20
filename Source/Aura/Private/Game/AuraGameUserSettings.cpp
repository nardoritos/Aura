// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "Game/AuraGameUserSettings.h"

#include "Aura/AuraLogChannels.h"
#include "Game/AuraGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetInternationalizationLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"
#include "UI/Widget/Subwidget/SettingRowWidget.h"
#include "UI/Widget/Subwidget/SettingRowWidget_AAMethod.h"
#include "UI/Widget/Subwidget/SettingRowWidget_AAQuality.h"
#include "UI/Widget/Subwidget/SettingRowWidget_EffectsQuality.h"
#include "UI/Widget/Subwidget/SettingRowWidget_FrameRate.h"
#include "UI/Widget/Subwidget/SettingRowWidget_GlobalIlluminationMethod.h"
#include "UI/Widget/Subwidget/SettingRowWidget_GlobalIlluminationQuality.h"
#include "UI/Widget/Subwidget/SettingRowWidget_Language.h"
#include "UI/Widget/Subwidget/SettingRowWidget_MasterVolume.h"
#include "UI/Widget/Subwidget/SettingRowWidget_MusicVolume.h"
#include "UI/Widget/Subwidget/SettingRowWidget_PostProcessingQuality.h"
#include "UI/Widget/Subwidget/SettingRowWidget_QualityPresets.h"
#include "UI/Widget/Subwidget/SettingRowWidget_ReflectionsQuality.h"
#include "UI/Widget/Subwidget/SettingRowWidget_Resolution.h"
#include "UI/Widget/Subwidget/SettingRowWidget_SFXVolume.h"
#include "UI/Widget/Subwidget/SettingRowWidget_ShadowsQuality.h"
#include "UI/Widget/Subwidget/SettingRowWidget_TexturesQuality.h"
#include "UI/Widget/Subwidget/SettingRowWidget_ViewDistance.h"
#include "UI/Widget/Subwidget/SettingRowWidget_VSync.h"
#include "UI/Widget/Subwidget/SettingRowWidget_WindowMode.h"

void UAuraGameUserSettings::LoadSettings(bool bForceReload)
{
	Super::LoadSettings(bForceReload);

	//TODO: Apply settings loaded from the config that aren't automatically applied (which ones, though?)

}

void UAuraGameUserSettings::RegisterNewSetting(ESettingType NewSetting, USettingRowWidget* SettingRowWidget)
{
	switch (NewSetting)
	{
	default:
		UE_LOG(LogAura, Warning, TEXT("WARNING: No Setting to register in AuraGameUserSettings"))
		break;
	case WindowMode:
		Register_WindowMode(SettingRowWidget);
		break;
	case Resolution:
		Register_Resolution(SettingRowWidget);
		break;
	case QualityPresets:
		Register_QualityPresets(SettingRowWidget);
		break;
	case GlobalIlluminationMethod:
		Register_GlobalIlluminationMethod(SettingRowWidget);
		break;
	case GlobalIlluminationQuality:
		Register_GlobalIlluminationQuality(SettingRowWidget);
		break;
	case ShadowsQuality:
		Register_Shadows(SettingRowWidget);
		break;
	case ViewDistance:
		Register_ViewDistance(SettingRowWidget);
		break;
	case TexturesQuality:
		Register_TextureQuality(SettingRowWidget);
		break;
	case EffectsQuality:
		Register_EffectsQuality(SettingRowWidget);
		break;
	case ReflectionsQuality:
		Register_ReflectionsQuality(SettingRowWidget);
		break;
	case PostProcessingQuality:
		Register_PostProcessingQuality(SettingRowWidget);
		break;
	case AntiAliasingMethod:
    	Register_AAMethod(SettingRowWidget);
    	break;
    case AntiAliasingQuality:
    	Register_AAQuality(SettingRowWidget);
    	break;
	case VSync:
		Register_VSync(SettingRowWidget);
		break;
	case FrameRate:
		Register_FrameRate(SettingRowWidget);
		break;
	case MasterVolume:
		Register_MasterVolume(SettingRowWidget);
		break;
	case MusicVolume:
		Register_MusicVolume(SettingRowWidget);
		break;
	case SoundEffects:
		Register_SFXVolume(SettingRowWidget);
		break;
	case Language:
		Register_Language(SettingRowWidget);
		break;
	}
	
}

USettingRowWidget* UAuraGameUserSettings::GetSettingInSlot(ESettingType SettingToFind)
{
	return SettingWidgets[SettingToFind];
}

void UAuraGameUserSettings::Register_WindowMode(USettingRowWidget* SettingRowWidget)
{
	if(USettingRowWidget_WindowMode* WindowModeWidget = Cast<USettingRowWidget_WindowMode>(SettingRowWidget))
	{
		SettingWidgets.Add(WindowMode, WindowModeWidget);
		WindowModeWidget->ReceiveInitialValue(GetFullscreenMode());
		WindowModeWidget->OnValueApplied.AddLambda([this, WindowModeWidget]()
		{
			UE_LOG(LogAura, Log, TEXT("Window Mode Applied"));
			SetFullscreenMode(WindowModeWidget->GetCurrentWindowMode());
			
			Update_Resolution();
		});
	}
}

void UAuraGameUserSettings::Register_Resolution(USettingRowWidget* SettingRowWidget)
{
	if(USettingRowWidget_Resolution* ResolutionWidget = Cast<USettingRowWidget_Resolution>(SettingRowWidget))
	{
		SettingWidgets.Add(Resolution, ResolutionWidget);
		ResolutionWidget->ReceiveInitialValue(GetScreenResolution());
		Update_Resolution();
		ResolutionWidget->OnValueApplied.AddLambda([this, ResolutionWidget]()
		{
			UE_LOG(LogAura, Log, TEXT("Resolution Changed"));
			SetScreenResolution(ResolutionWidget->GetCurrentResolution());
		
			Update_Resolution();
		});
	}
}

void UAuraGameUserSettings::Register_QualityPresets(USettingRowWidget* SettingRowWidget)
{
	if(USettingRowWidget_QualityPresets* QualityPresetsWidget = Cast<USettingRowWidget_QualityPresets>(SettingRowWidget))
	{
		SettingWidgets.Add(QualityPresets, QualityPresetsWidget);
		QualityPresetsWidget->ReceiveInitialValue(GetOverallScalabilityLevel());
		QualityPresetsWidget->OnValueChanged.AddLambda([this, QualityPresetsWidget]
			(ESettingType SettingType, bool bIsValueDifferentThanCurrentAppliedValue)
		{
			// Automatic updaters to when a Quality Preset is directly set 
			const int32 CurrentQualityPreset = QualityPresetsWidget->GetCurrentQualityPreset();
			// Global Illumination Quality
			if (USettingRowWidget_GlobalIlluminationQuality* GlobalIlluminationQualityWidget =
				Cast<USettingRowWidget_GlobalIlluminationQuality>(GetSettingInSlot(GlobalIlluminationQuality)))
			{
				GlobalIlluminationQualityWidget->SetCurrentGlobalIlluminationQuality(CurrentQualityPreset);
			}
			// Shadows Quality
			if (USettingRowWidget_ShadowsQuality* ShadowsWidget = Cast<USettingRowWidget_ShadowsQuality>(GetSettingInSlot(ShadowsQuality)))
			{
				ShadowsWidget->SetCurrentShadowsQuality(CurrentQualityPreset);
			}
			// AA Quality
			if (USettingRowWidget_AAQuality* AAQualityWidget = Cast<USettingRowWidget_AAQuality>(GetSettingInSlot(AntiAliasingQuality)))
			{
				AAQualityWidget->SetCurrentAAQuality(CurrentQualityPreset);
			}
			// View Distance
			if (USettingRowWidget_ViewDistance* ViewDistanceWidget = Cast<USettingRowWidget_ViewDistance>(GetSettingInSlot(ViewDistance)))
			{
				ViewDistanceWidget->SetCurrentViewDistance(CurrentQualityPreset);
			}
			// Textures Quality
			if (USettingRowWidget_TexturesQuality* TexturesQualityWidget = Cast<USettingRowWidget_TexturesQuality>(GetSettingInSlot(TexturesQuality)))
			{
				TexturesQualityWidget->SetCurrentTexturesQuality(CurrentQualityPreset);
			}
			// Effects Quality
			if (USettingRowWidget_EffectsQuality* EffectsWidget = Cast<USettingRowWidget_EffectsQuality>(GetSettingInSlot(EffectsQuality)))
			{
				EffectsWidget->SetCurrentEffectsQuality(CurrentQualityPreset);
			}
			// ReflectionsQuality
			if (USettingRowWidget_ReflectionsQuality* ReflectionsWidget = Cast<USettingRowWidget_ReflectionsQuality>(GetSettingInSlot(ReflectionsQuality)))
			{
				ReflectionsWidget->SetCurrentReflectionsQuality(CurrentQualityPreset);
			}
			// Post Processing Quality
			if (USettingRowWidget_PostProcessingQuality* PostProcessingWidget = Cast<USettingRowWidget_PostProcessingQuality>(GetSettingInSlot(PostProcessingQuality)))
			{
				PostProcessingWidget->SetCurrentPostProcessingQuality(CurrentQualityPreset);
			}
		});
		QualityPresetsWidget->OnValueApplied.AddLambda([this, QualityPresetsWidget]()
		{
			UE_LOG(LogAura, Log, TEXT("Quality Preset Applied"));
			SetOverallScalabilityLevel(QualityPresetsWidget->GetCurrentQualityPreset());
			
			// Update_GlobalIlluminationQuality(true);
			// Update_ShadowsQuality(true);
			// Update_ViewDistance(true);
		}); 
	}
}

void UAuraGameUserSettings::Register_GlobalIlluminationMethod(USettingRowWidget* SettingRowWidget)
{
	if(USettingRowWidget_GlobalIlluminationMethod* GlobalIlluminationMethodWidget =
		Cast<USettingRowWidget_GlobalIlluminationMethod>(SettingRowWidget))
	{
		SettingWidgets.Add(ESettingType::GlobalIlluminationMethod, GlobalIlluminationMethodWidget);
		GlobalIlluminationMethodWidget->ReceiveInitialValue(GetGlobalIlluminationMethod());
		GlobalIlluminationMethodWidget->OnValueApplied.AddLambda([this, GlobalIlluminationMethodWidget]()
		{
			UE_LOG(LogAura, Log, TEXT("GI Method Applied"));
			SetGlobalIlluminationMethod(GlobalIlluminationMethodWidget->GetCurrentGlobalIlluminationMethod());
			Update_GlobalIlluminationQuality(true);
			
		});
	}
}

void UAuraGameUserSettings::Register_GlobalIlluminationQuality(USettingRowWidget* SettingRowWidget)
{
	if(USettingRowWidget_GlobalIlluminationQuality* GlobalIlluminationQualityWidget =
		Cast<USettingRowWidget_GlobalIlluminationQuality>(SettingRowWidget))
	{
		SettingWidgets.Add(GlobalIlluminationQuality, GlobalIlluminationQualityWidget);
		GlobalIlluminationQualityWidget->ReceiveInitialValue(GetGlobalIlluminationQuality());
		Update_GlobalIlluminationQuality();
		GlobalIlluminationQualityWidget->OnValueApplied.AddLambda([this, GlobalIlluminationQualityWidget]()
		{
			UE_LOG(LogAura, Log, TEXT("Quality Preset Applied"));
			SetGlobalIlluminationQuality(GlobalIlluminationQualityWidget->GetCurrentGlobalIlluminationQuality());
		});
	}
}

void UAuraGameUserSettings::Register_Shadows(USettingRowWidget* SettingRowWidget)
{
	if (USettingRowWidget_ShadowsQuality* ShadowsWidget = Cast<USettingRowWidget_ShadowsQuality>(SettingRowWidget))
	{
		SettingWidgets.Add(ShadowsQuality, ShadowsWidget);
		ShadowsWidget->ReceiveInitialValue(GetShadowQuality());
		ShadowsWidget->OnValueApplied.AddLambda([this, ShadowsWidget]()
		{
			UE_LOG(LogAura, Log, TEXT("Shadows Applied"));
			SetShadowQuality(ShadowsWidget->GetCurrentShadowsQuality());
		});
	}
}

void UAuraGameUserSettings::Register_ViewDistance(USettingRowWidget* SettingRowWidget)
{
	if (USettingRowWidget_ViewDistance* ViewDistanceWidget = Cast<USettingRowWidget_ViewDistance>(SettingRowWidget))
	{
		SettingWidgets.Add(ViewDistance, ViewDistanceWidget);
		ViewDistanceWidget->ReceiveInitialValue(GetViewDistanceQuality());
		ViewDistanceWidget->OnValueApplied.AddLambda([this, ViewDistanceWidget]()
		{
			UE_LOG(LogAura, Log, TEXT("View Distance Applied"));
			SetViewDistanceQuality(ViewDistanceWidget->GetCurrentViewDistance());
		});
	}
}

void UAuraGameUserSettings::Register_TextureQuality(USettingRowWidget* SettingRowWidget)
{
	if (USettingRowWidget_TexturesQuality* TexturesQualityWidget = Cast<USettingRowWidget_TexturesQuality>(SettingRowWidget))
	{
		SettingWidgets.Add(TexturesQuality, TexturesQualityWidget);
		TexturesQualityWidget->ReceiveInitialValue(GetTextureQuality());
		TexturesQualityWidget->OnValueApplied.AddLambda([this, TexturesQualityWidget]()
		{
			UE_LOG(LogAura, Log, TEXT("Textures Quality Applied"));
			SetTextureQuality(TexturesQualityWidget->GetCurrentTexturesQuality());
		});
	}
}

void UAuraGameUserSettings::Register_EffectsQuality(USettingRowWidget* SettingRowWidget)
{
	if (USettingRowWidget_EffectsQuality* EffectsQualityWidget = Cast<USettingRowWidget_EffectsQuality>(SettingRowWidget))
	{
		SettingWidgets.Add(EffectsQuality, EffectsQualityWidget);
		EffectsQualityWidget->ReceiveInitialValue(GetVisualEffectQuality());
		EffectsQualityWidget->OnValueApplied.AddLambda([this, EffectsQualityWidget]()
		{
			UE_LOG(LogAura, Log, TEXT("Effects Quality Applied"));
			SetVisualEffectQuality(EffectsQualityWidget->GetCurrentEffectsQuality());
		});
	}
}

void UAuraGameUserSettings::Register_ReflectionsQuality(USettingRowWidget* SettingRowWidget)
{
	if (USettingRowWidget_ReflectionsQuality* ReflectionsQualityWidget = Cast<USettingRowWidget_ReflectionsQuality>(SettingRowWidget))
	{
		SettingWidgets.Add(ReflectionsQuality, ReflectionsQualityWidget);
		ReflectionsQualityWidget->ReceiveInitialValue(GetReflectionQuality());
		ReflectionsQualityWidget->OnValueApplied.AddLambda([this, ReflectionsQualityWidget]()
		{
			UE_LOG(LogAura, Log, TEXT("Reflections Quality Applied"));
			SetReflectionQuality(ReflectionsQualityWidget->GetCurrentReflectionsQuality());
		});
	}
}

void UAuraGameUserSettings::Register_PostProcessingQuality(USettingRowWidget* SettingRowWidget)
{
	if (USettingRowWidget_PostProcessingQuality* PostProcessingQualityWidget = Cast<USettingRowWidget_PostProcessingQuality>(SettingRowWidget))
	{
		SettingWidgets.Add(PostProcessingQuality, PostProcessingQualityWidget);
		PostProcessingQualityWidget->ReceiveInitialValue(GetPostProcessingQuality());
		PostProcessingQualityWidget->OnValueApplied.AddLambda([this, PostProcessingQualityWidget]()
		{
			UE_LOG(LogAura, Log, TEXT("PostProcessing Quality Applied"));
			SetPostProcessingQuality(PostProcessingQualityWidget->GetCurrentPostProcessingQuality());
		});
	}
}

void UAuraGameUserSettings::Register_AAMethod(USettingRowWidget* SettingRowWidget)
{
	if (USettingRowWidget_AAMethod* AAMethodWidget = Cast<USettingRowWidget_AAMethod>(SettingRowWidget))
	{
		SettingWidgets.Add(AntiAliasingMethod, AAMethodWidget);
		AAMethodWidget->ReceiveInitialValue(GetAntiAliasingMethod());
		AAMethodWidget->OnValueApplied.AddLambda([this, AAMethodWidget]()
		{
			UE_LOG(LogAura, Log, TEXT("Anti Aliasing Method Applied"))
			SetAntiAliasingMethod(AAMethodWidget->GetCurrentAAMethod());
			Update_AAQuality(true);
		});
	}
}

void UAuraGameUserSettings::Register_AAQuality(USettingRowWidget* SettingRowWidget)
{
	if (USettingRowWidget_AAQuality* AAQualityWidget = Cast<USettingRowWidget_AAQuality>(SettingRowWidget))
	{
		SettingWidgets.Add(AntiAliasingQuality, AAQualityWidget);
		AAQualityWidget->ReceiveInitialValue(GetAntiAliasingQuality());
		Update_AAQuality();
		AAQualityWidget->OnValueApplied.AddLambda([this, AAQualityWidget]()
		{
			UE_LOG(LogAura, Log, TEXT("Anti Aliasing Quality Applied"))
			SetAntiAliasingQuality(AAQualityWidget->GetCurrentAAQuality());
		});
	}
}

void UAuraGameUserSettings::Register_VSync(USettingRowWidget* SettingRowWidget)
{
	if (USettingRowWidget_VSync* VSyncWidget = Cast<USettingRowWidget_VSync>(SettingRowWidget))
	{
		SettingWidgets.Add(VSync, VSyncWidget);
		VSyncWidget->ReceiveInitialValue(VSyncWidget->GetCurrentVSync());
		VSyncWidget->OnValueApplied.AddLambda([this, VSyncWidget]()
		{
			UE_LOG(LogAura, Log, TEXT("VSync Applied"));
			SetVSyncEnabled(VSyncWidget->GetCurrentVSync());
			Update_FrameRate();
		});
	}
}

void UAuraGameUserSettings::Register_FrameRate(USettingRowWidget* SettingRowWidget)
{
	if (USettingRowWidget_FrameRate* FrameRateWidget = Cast<USettingRowWidget_FrameRate>(SettingRowWidget))
	{
		SettingWidgets.Add(FrameRate, FrameRateWidget);
		FrameRateWidget->ReceiveInitialValue(GetFrameRateLimit());
		Update_FrameRate();
		FrameRateWidget->OnValueApplied.AddLambda([this, FrameRateWidget]()
		{
			UE_LOG(LogAura, Log, TEXT("FrameRate Applied"));
			SetFrameRateLimit(FrameRateWidget->GetCurrentFrameRate());
		});
	}
}

void UAuraGameUserSettings::Register_MasterVolume(USettingRowWidget* SettingRowWidget)
{
	if (USettingRowWidget_MasterVolume* MasterVolumeWidget = Cast<USettingRowWidget_MasterVolume>(SettingRowWidget))
	{
		SettingWidgets.Add(MasterVolume, MasterVolumeWidget);
		MasterVolumeWidget->ReceiveInitialValue(GetMasterVolume());
		MasterVolumeWidget->OnValueApplied.AddLambda([this, MasterVolumeWidget]()
		{
			UE_LOG(LogAura, Log, TEXT("Master Volume applied"));
			SetMasterVolume(MasterVolumeWidget->GetCurrentMasterVolume());
		});
	}
}

void UAuraGameUserSettings::Register_SFXVolume(USettingRowWidget* SettingRowWidget)
{
	if (USettingRowWidget_SFXVolume* SFXVolumeWidget = Cast<USettingRowWidget_SFXVolume>(SettingRowWidget))
	{
		SettingWidgets.Add(SoundEffects, SFXVolumeWidget);
		SFXVolumeWidget->ReceiveInitialValue(GetSFXVolume());
		SFXVolumeWidget->OnValueApplied.AddLambda([this, SFXVolumeWidget]()
		{
			UE_LOG(LogAura, Log, TEXT("SFX Volume applied"));
			SetSFXVolume(SFXVolumeWidget->GetCurrentSFXVolume());
		});
	}
}

void UAuraGameUserSettings::Register_MusicVolume(USettingRowWidget* SettingRowWidget)
{
	if (USettingRowWidget_MusicVolume* MusicVolumeWidget = Cast<USettingRowWidget_MusicVolume>(SettingRowWidget))
	{
		SettingWidgets.Add(MusicVolume, MusicVolumeWidget);
		MusicVolumeWidget->ReceiveInitialValue(GetMusicVolume());
		MusicVolumeWidget->OnValueApplied.AddLambda([this, MusicVolumeWidget]()
		{
			UE_LOG(LogAura, Log, TEXT("Music Volume applied"));
			SetMusicVolume(MusicVolumeWidget->GetCurrentMusicVolume());
		});
	}
}

void UAuraGameUserSettings::Register_Language(USettingRowWidget* SettingRowWidget)
{
	if (USettingRowWidget_Language* LanguageWidget  = Cast<USettingRowWidget_Language>(SettingRowWidget))
	{
		SettingWidgets.Add(Language, LanguageWidget);
		LanguageWidget->OnValueApplied.AddLambda([this, LanguageWidget]()
		{
			UE_LOG(LogAura, Log, TEXT("Language applied"));
			UKismetInternationalizationLibrary::SetCurrentLanguage(LanguageWidget->GetCurrentLanguage());
		});
		LanguageWidget->ReceiveInitialValue(UKismetInternationalizationLibrary::GetCurrentLanguage());
		
	}
}

void UAuraGameUserSettings::Update_Resolution(const bool bOverrideResolution)
{
	if (USettingRowWidget_Resolution* ResolutionWidget = Cast<USettingRowWidget_Resolution>(GetSettingInSlot(Resolution)))
	{
		UE_LOG(LogAura, Log, TEXT("Resolution updated"))
		if (USettingRowWidget_WindowMode* WindowModeWidget = Cast<USettingRowWidget_WindowMode>(GetSettingInSlot(WindowMode)))
		{
			if (WindowModeWidget->GetCurrentWindowMode() == EWindowMode::Type::WindowedFullscreen)
			{
				ResolutionWidget->Disable();
				TArray<FIntPoint> SupportedFullscreenResolutions;
		
				UKismetSystemLibrary::GetSupportedFullscreenResolutions(SupportedFullscreenResolutions);
				// The player's actual display size
				const FIntPoint ScreenSize = FIntPoint(GSystemResolution.ResX, GSystemResolution.ResY);
				UE_LOG(LogAura, Warning, TEXT("Player Screen is %ix%i long"), ScreenSize.X, ScreenSize.Y)
	
				// Clearing resolutions bigger than the Screen Resolution
				for (FIntPoint CurrentRes : SupportedFullscreenResolutions)
				{
					if (CurrentRes.X > ScreenSize.X || CurrentRes.Y > ScreenSize.Y)
					{
						SupportedFullscreenResolutions.Remove(CurrentRes);
					}
				}
	
				ResolutionWidget->SetAvailableResolutions(SupportedFullscreenResolutions);
				ResolutionWidget->SetCurrentResolution(GetScreenResolution());				
			}
			else
			{
				ResolutionWidget->Enable();
				if (bOverrideResolution)
				{
					ResolutionWidget->SetCurrentResolution(GetScreenResolution());
				}
			}
		}
	}
}

void UAuraGameUserSettings::Update_GlobalIlluminationQuality(const bool bOverrideGlobalIlluminationQuality) 
{
	if (USettingRowWidget_GlobalIlluminationQuality* GlobalIlluminationQualityWidget =
		Cast<USettingRowWidget_GlobalIlluminationQuality>(GetSettingInSlot(GlobalIlluminationQuality)))
	{
		UE_LOG(LogAura, Log, TEXT("GlobalIlluminationQuality updated"))
		if (USettingRowWidget_GlobalIlluminationMethod* GlobalIlluminationMethodWidget =
			Cast<USettingRowWidget_GlobalIlluminationMethod>(GetSettingInSlot(GlobalIlluminationMethod)))
		{
			if (GlobalIlluminationMethodWidget->GetCurrentGlobalIlluminationMethod() == 0)
			{
				GlobalIlluminationQualityWidget->Disable();
				GlobalIlluminationQualityWidget->SetCurrentGlobalIlluminationQuality(-1);
			}
			else
			{
				GlobalIlluminationQualityWidget->Enable();
				if (bOverrideGlobalIlluminationQuality)
				{
					GlobalIlluminationQualityWidget->SetCurrentGlobalIlluminationQuality(GetGlobalIlluminationQuality());
				}
			}
		}
	}
}

void UAuraGameUserSettings::Update_AAQuality(const bool bOverrideAAQuality)
{
	if (USettingRowWidget_AAQuality* AAQualityWidget = Cast<USettingRowWidget_AAQuality>(GetSettingInSlot(AntiAliasingQuality)))
	{
		UE_LOG(LogAura, Log, TEXT("AAQuality updated"))
		if (USettingRowWidget_AAMethod* AAMethodWidget = Cast<USettingRowWidget_AAMethod>(GetSettingInSlot(AntiAliasingMethod)))
		{
			if (AAMethodWidget->GetCurrentAAMethod() == 0)
			{
				AAQualityWidget->Disable();
				AAQualityWidget->SetCurrentAAQuality(-1);
				
			}
			else
			{
				AAQualityWidget->Enable();
				if (bOverrideAAQuality)
				{
					AAQualityWidget->SetCurrentAAQuality(GetAntiAliasingQuality());
				}
			}
		}
	}
}

void UAuraGameUserSettings::Update_FrameRate(const bool bOverrideFrameRate)
{
	if (USettingRowWidget_FrameRate* FrameRateWidget = Cast<USettingRowWidget_FrameRate>(GetSettingInSlot(FrameRate)))
	{
		UE_LOG(LogAura, Log, TEXT("FrameRate updated"))
		if (USettingRowWidget_VSync* VSyncWidget = Cast<USettingRowWidget_VSync>(GetSettingInSlot(VSync)))
		{
			if (!VSyncWidget->GetCurrentVSync())
			{
				FrameRateWidget->Disable();
				FrameRateWidget->SetCurrentFrameRate(GetFrameRateLimit());				
			}
			else
			{
				FrameRateWidget->Enable();
				if (bOverrideFrameRate)
				{
					FrameRateWidget->SetCurrentFrameRate(GetFrameRateLimit());
				}
			}
		}
	}
}

int32 UAuraGameUserSettings::GetGlobalIlluminationMethod()
{
	int32 ConsoleValue = IConsoleManager::Get().FindConsoleVariable(TEXT("r.DynamicGlobalIlluminationMethod"))->GetInt();
	if (ConsoleValue == ConfigGlobalIlluminationMethod)
	{
		return ConfigGlobalIlluminationMethod;
	}
	// If they aren't the same value, the variable has initialized at a different value than the console
	SetGlobalIlluminationMethod(ConsoleValue);
	return ConsoleValue;
}

void UAuraGameUserSettings::SetGlobalIlluminationMethod(const int32 InMethod)
{
	ConfigGlobalIlluminationMethod = InMethod;
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.DynamicGlobalIlluminationMethod"))->Set(ConfigGlobalIlluminationMethod);
}

int32 UAuraGameUserSettings::GetAntiAliasingMethod()
{
	int32 ConsoleValue = IConsoleManager::Get().FindConsoleVariable(TEXT("r.AntiAliasingMethod"))->GetInt();
	if (ConsoleValue == ConfigAntiAliasingMethod)
	{
		return ConfigAntiAliasingMethod;
	}
	// If they aren't the same value, the variable has initialized at a different value than the console
	SetAntiAliasingMethod(ConsoleValue);
	return ConsoleValue;
}

void UAuraGameUserSettings::SetAntiAliasingMethod(const int32 InMethod)
{
	ConfigAntiAliasingMethod = InMethod;
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.AntiAliasingMethod"))->Set(ConfigAntiAliasingMethod);
}

void UAuraGameUserSettings::SetMasterVolume(const float InMasterVolume)
{
	ConfigMasterVolume = InMasterVolume;
	OnMasterVolumeApplied.Broadcast(ConfigMasterVolume);
}

void UAuraGameUserSettings::SetSFXVolume(const float InSFXVolume)
{
	ConfigSFXVolume = InSFXVolume;
	OnSFXVolumeApplied.Broadcast(ConfigSFXVolume);
}

void UAuraGameUserSettings::SetMusicVolume(const float InMusicVolume)
{
	ConfigMusicVolume = InMusicVolume;
	OnMusicVolumeApplied.Broadcast(ConfigMusicVolume);
}