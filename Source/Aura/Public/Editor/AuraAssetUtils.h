// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "AuraAssetUtils.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraAssetUtils : public UAssetActionUtility
{
	GENERATED_BODY()

	UAuraAssetUtils();
	
	UFUNCTION(BlueprintCallable, CallInEditor)
	void SaveThumbnails();

};
