// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "Game/AuraSaveGame.h"

FSavedMap UAuraSaveGame::GetSavedMapWithMapName(const FString& InMapName)
{
	for (const FSavedMap& Map : SavedMaps)
		if (Map.MapAssetName == InMapName)
			return Map;

	return FSavedMap();
}

bool UAuraSaveGame::HasMap(const FString& InMapName)
{
	for (const FSavedMap& Map : SavedMaps)
		if (Map.MapAssetName == InMapName)
			return true;

	return false;
}
