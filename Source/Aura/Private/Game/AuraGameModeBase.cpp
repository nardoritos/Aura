// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "Game/AuraGameModeBase.h"

#include "EngineUtils.h"
#include "Actor/MinimapReceiverComponent.h"
#include "Aura/AuraLogChannels.h"
#include "Character/AuraCharacter.h"
#include "Game/AuraGameInstance.h"
#include "Game/AuraGameUserSettings.h"
#include "Game/AuraSaveGame.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/SaveInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void AAuraGameModeBase::SaveSlotData(const UMVVM_LoadSlot* LoadSlot, const int32 SlotIndex) const
{
	DeleteSlot(LoadSlot->GetLoadSlotName(), SlotIndex);
	
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);

	UAuraSaveGame* LoadScreenSaveGame = Cast<UAuraSaveGame>(SaveGameObject);
	LoadScreenSaveGame->PlayerName = LoadSlot->GetPlayerName();
	LoadScreenSaveGame->SaveSlotStatus = Taken;
	LoadScreenSaveGame->MapName = LoadSlot->GetMapName();
	LoadScreenSaveGame->MapAssetName = LoadSlot->MapAssetName;
	LoadScreenSaveGame->PlayerStartTag = LoadSlot->PlayerStartTag;
	
	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlot->GetLoadSlotName(), SlotIndex);
}

UAuraSaveGame* AAuraGameModeBase::GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const
{
	USaveGame* SaveGameObject = nullptr;
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		SaveGameObject = UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex);
	}
	else
	{
		SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	}
	UAuraSaveGame* LoadScreenSaveGame = Cast<UAuraSaveGame>(SaveGameObject);
	return LoadScreenSaveGame;
}

void AAuraGameModeBase::DeleteSlot(const FString& SlotName, int32 SlotIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
	}
}

UAuraSaveGame* AAuraGameModeBase::RetrieveInGameSaveData() const
{
	const UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());

	const FString InGameLoadSlotName = AuraGameInstance->LoadSlotName;
	const int32 InGameLoadSlotIndex = AuraGameInstance->LoadSlotIndex;

	return GetSaveSlotData(InGameLoadSlotName, InGameLoadSlotIndex);
}

void AAuraGameModeBase::SaveInGameProgressData(UAuraSaveGame* SaveObject)
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());

	const FString InGameLoadSlotName = AuraGameInstance->LoadSlotName;
	const int32 InGameLoadSlotIndex = AuraGameInstance->LoadSlotIndex;
	AuraGameInstance->PlayerStartTag = SaveObject->PlayerStartTag;
	
	UGameplayStatics::SaveGameToSlot(SaveObject, InGameLoadSlotName, InGameLoadSlotIndex);
}

void AAuraGameModeBase::SaveWorldState(UWorld* World, const FString& DestinationMapAssetName) const
{
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	check(AuraGameInstance);

	if (UAuraSaveGame* SaveGame = GetSaveSlotData(AuraGameInstance->LoadSlotName, AuraGameInstance->LoadSlotIndex))
	{
		if (DestinationMapAssetName != FString(""))
		{
			SaveGame->MapAssetName = DestinationMapAssetName;
			SaveGame->MapName = GetMapNameFromMapAssetName(DestinationMapAssetName); 
		}
		if (!SaveGame->HasMap(WorldName))
		{
			FSavedMap NewSavedMap;
			NewSavedMap.MapAssetName = WorldName;
			SaveGame->SavedMaps.Add(NewSavedMap);
		}
		FSavedMap SavedMap = SaveGame->GetSavedMapWithMapName(WorldName);
		SavedMap.SavedActors.Empty();

		for (FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;

			if (!IsValid(Actor) || !Actor->Implements<USaveInterface>()) continue;

			FSavedActor SavedActor;
			SavedActor.ActorName = Actor->GetFName();
			SavedActor.Transform = Actor->GetTransform();

			FMemoryWriter MemoryWriter(SavedActor.Bytes);

			FObjectAndNameAsStringProxyArchive Archive(MemoryWriter, true);
			Archive.ArIsSaveGame = true;

			Actor->Serialize(Archive);

			SavedMap.SavedActors.AddUnique(SavedActor);
		}
		for (FSavedMap& MapToReplace : SaveGame->SavedMaps)
		{
			if (MapToReplace.MapAssetName == WorldName)
			{
				MapToReplace = SavedMap;
			}
		}
		UGameplayStatics::SaveGameToSlot(SaveGame, AuraGameInstance->LoadSlotName, AuraGameInstance->LoadSlotIndex);
	}
}

void AAuraGameModeBase::LoadWorldState(UWorld* World) const
{
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	check(AuraGameInstance);

	if (UGameplayStatics::DoesSaveGameExist(AuraGameInstance->LoadSlotName, AuraGameInstance->LoadSlotIndex))
	{
		UAuraSaveGame* SaveGame = Cast<UAuraSaveGame>(UGameplayStatics::LoadGameFromSlot(AuraGameInstance->LoadSlotName, AuraGameInstance->LoadSlotIndex));
		if (SaveGame == nullptr)
		{
			UE_LOG(LogAura, Error, TEXT("Failed to load Slot"));
			return;
		}
		for (FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;

			if (!Actor->Implements<USaveInterface>()) continue;

			for (FSavedActor SavedActor : SaveGame->GetSavedMapWithMapName(WorldName).SavedActors)
			{
				if (SavedActor.ActorName == Actor->GetFName())
				{
					if (ISaveInterface::Execute_ShouldLoadTransform(Actor))
					{
						Actor->SetActorTransform(SavedActor.Transform);
					}

					FMemoryReader MemoryReader(SavedActor.Bytes);
					
					FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
					Archive.ArIsSaveGame = true;
					
					Actor->Serialize(Archive); // Converts binary bytes back into variables

					ISaveInterface::Execute_LoadActor(Actor);
				}
			}
		}
	}
}

void AAuraGameModeBase::TravelToMap(UMVVM_LoadSlot* Slot)
{
	const FString SlotName = Slot->GetLoadSlotName();
		
	UGameplayStatics::OpenLevelBySoftObjectPtr(Slot, Maps.FindChecked(Slot->GetMapName()).Map);
}

FString AAuraGameModeBase::GetMapNameFromMapAssetName(const FString& MapAssetName) const
{
	for (auto& Map : Maps)
	{
		if (Map.Value.Map.ToSoftObjectPath().GetAssetName() == MapAssetName)
		{
			return Map.Key;
		}
	}
	return FString();
}

AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	const UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Actors);

	if (Actors.Num() > 0)
	{
		AActor* SelectedActor = Actors[0];

		for (AActor* Actor : Actors)
		{
			if(APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
			{
				if (PlayerStart->PlayerStartTag == AuraGameInstance->PlayerStartTag)
				{
					
					SelectedActor = PlayerStart;
					break;
				}
			}
			
		}
		UE_LOG(LogAura, Log, TEXT("Chose PlayerStart %s"), *AuraGameInstance->PlayerStartTag.ToString())
		return SelectedActor;
	}
	UE_LOG(LogAura, Error, TEXT("Failed to find PlayerStart"));
	return nullptr;
}

void AAuraGameModeBase::PlayerDied(const ACharacter* DeadCharacter) const
{
	UAuraSaveGame* SaveGame = RetrieveInGameSaveData();
	if (!IsValid(SaveGame)) return;

	UGameplayStatics::OpenLevel(DeadCharacter, FName(SaveGame->MapAssetName));
	
}

void AAuraGameModeBase::LoadAudioSettings() const
{
	if (UAuraGameUserSettings* AuraGUS = Cast<UAuraGameUserSettings>(GEngine->GetGameUserSettings()))
	{
		AuraGUS->OnMasterVolumeApplied.AddUObject(this, &AAuraGameModeBase::ApplyMasterVolume);
		AuraGUS->OnSFXVolumeApplied.AddUObject(this, &AAuraGameModeBase::ApplySFXVolume);
		AuraGUS->OnMusicVolumeApplied.AddUObject(this, &AAuraGameModeBase::ApplyMusicVolume);
	}
}

void AAuraGameModeBase::TryApplyMinimapToPlayers()
{
	FString WorldName = GetWorld()->GetMapName();
	WorldName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	CurrentMap = WorldName;
	
	TArray<AActor*> OutActors;
 	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAuraCharacter::StaticClass(), OutActors);

	for (AActor* Actor : OutActors)
	{
		if (AAuraCharacter* Player = Cast<AAuraCharacter>(Actor); Player->Implements<UPlayerInterface>())
		{
			for (auto Pair : Maps)
			{
				if (Pair.Value.InternalName == CurrentMap)
				{
					UMinimapReceiverComponent* MinimapComponent = IPlayerInterface::Execute_GetMinimapComponent(Player);
					MinimapComponent->SetMinimapInstance(Pair.Value.MinimapInstance);
					break;
				}
			}
		}
	}
}

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	LoadAudioSettings();
	
//	Maps.Add(DefaultMapName, FMapSetting{DefaultMapInternalName, DefaultMap, nullptr});

	TryApplyMinimapToPlayers();
}

void AAuraGameModeBase::ApplyMasterVolume(const float InMasterVolume) const
{
	UGameplayStatics::SetSoundMixClassOverride(
		GetWorld(),
		MasterSoundMix,
		MasterSoundClass,
		InMasterVolume);
	UGameplayStatics::PushSoundMixModifier(GetWorld(), MasterSoundMix);
}

void AAuraGameModeBase::ApplySFXVolume(const float InSFXVolume) const
{

	UGameplayStatics::SetSoundMixClassOverride(
		GetWorld(),
		SFXSoundMix,
		SFXSoundClass,
		InSFXVolume);
	UGameplayStatics::PushSoundMixModifier(GetWorld(), SFXSoundMix);

}

void AAuraGameModeBase::ApplyMusicVolume(const float InMusicVolume) const
{
	UGameplayStatics::SetSoundMixClassOverride(
		GetWorld(),
		MusicSoundMix,
		MusicSoundClass,
		InMusicVolume);
	UGameplayStatics::PushSoundMixModifier(GetWorld(), MusicSoundMix);
}
