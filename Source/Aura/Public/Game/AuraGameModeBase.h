// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

class UAuraSaveGame;
class USaveGame;
class UMVVM_LoadSlot;
class UAbilityInfo;
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
	TObjectPtr<UAbilityInfo> AbilityInfo;

	void SaveSlotData(const UMVVM_LoadSlot* LoadSlot, const int32 SlotIndex) const;
	UAuraSaveGame* GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const;
	static void DeleteSlot(const FString& SlotName, int32 SlotIndex);
	UAuraSaveGame* RetrieveInGameSaveData() const;
	void SaveInGameProgressData(UAuraSaveGame* SaveObject);
	
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

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
protected:
	virtual void BeginPlay() override;
};
