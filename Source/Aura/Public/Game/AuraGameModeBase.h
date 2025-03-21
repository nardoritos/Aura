// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

class ULootTiers;
class UAuraSaveGame;
class USaveGame;
class UMVVM_LoadSlot;
class AAbilityInfo;
class UCharacterClassInfo;

/**
 * 
 */
UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, Category = "Aura|Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	UPROPERTY(EditDefaultsOnly, Category = "Aura|Ability Info")
	TObjectPtr<UDataTable> AbilityInfo;

	UPROPERTY(EditDefaultsOnly, Category = "Aura|Loot Tiers")
	TObjectPtr<ULootTiers> LootTiers;

	void SaveSlotData(const UMVVM_LoadSlot* LoadSlot, const int32 SlotIndex) const;
	UAuraSaveGame* GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const;
	static void DeleteSlot(const FString& SlotName, int32 SlotIndex);
	UAuraSaveGame* RetrieveInGameSaveData() const;
	void SaveInGameProgressData(UAuraSaveGame* SaveObject);

	void SaveWorldState(UWorld* World, const FString& DestinationMapAssetName = FString("")) const;
	void LoadWorldState(UWorld* World) const;
	
	void TravelToMap(UMVVM_LoadSlot* Slot);
	
	UPROPERTY(EditDefaultsOnly, Category = "Aura|Save")
	TSubclassOf<USaveGame> LoadScreenSaveGameClass;

	UPROPERTY(EditDefaultsOnly, Category = "Aura|Maps")
	FString DefaultMapName;

	UPROPERTY(EditDefaultsOnly, Category = "Aura|Maps")
	TSoftObjectPtr<UWorld> DefaultMap;

	UPROPERTY(EditDefaultsOnly, Category = "Aura|Maps")
	FName DefaultPlayerStartTag;
	
	UPROPERTY(EditDefaultsOnly, Category = "Aura|Maps")
	TMap<FString, TSoftObjectPtr<UWorld>> Maps;

	FString GetMapNameFromMapAssetName(const FString& MapAssetName) const;
	
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	void PlayerDied(const ACharacter* DeadCharacter) const;

	void LoadAudioSettings() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Audio|Master")
	TObjectPtr<USoundMix> MasterSoundMix;

	UPROPERTY(EditDefaultsOnly, Category = "Audio|Master")
	TObjectPtr<USoundClass> MasterSoundClass;

	UPROPERTY(EditDefaultsOnly, Category = "Audio|SFX")
	TObjectPtr<USoundMix> SFXSoundMix;

	UPROPERTY(EditDefaultsOnly, Category = "Audio|SFX")
	TObjectPtr<USoundClass> SFXSoundClass;

	UPROPERTY(EditDefaultsOnly, Category = "Audio|Music")
	TObjectPtr<USoundMix> MusicSoundMix;

	UPROPERTY(EditDefaultsOnly, Category = "Audio|Music")
	TObjectPtr<USoundClass> MusicSoundClass;


	// Manual Appliers
	void ApplyMasterVolume(const float InMasterVolume) const;
	void ApplySFXVolume(const float InSFXVolume) const;
	void ApplyMusicVolume(const float InMusicVolume) const;
	

	
};
