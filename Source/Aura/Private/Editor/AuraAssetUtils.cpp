// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "Editor/AuraAssetUtils.h"

#include "IContentBrowserSingleton.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"

UAuraAssetUtils::UAuraAssetUtils()
{
	bIsActionForBlueprints = true;
	SupportedClasses.Add(AActor::StaticClass());
}

void UAuraAssetUtils::SaveThumbnails()
{
	FString BaseExportPath = FPaths::ProjectSavedDir() + "Thumbnails/";
	
	TArray<FAssetData> SelectedAssets;
	IContentBrowserSingleton::Get().GetSelectedAssets(SelectedAssets);
	
	for (auto CurrentAsset : SelectedAssets)
	{
		auto ObjectPath = CurrentAsset.GetSoftObjectPath();
		
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(ObjectPath);
		UObject* MyObject = AssetData.GetAsset();
		if (MyObject)
		{
			FObjectThumbnail* ObjectThumbnail = ThumbnailTools::GenerateThumbnailForObjectToSaveToDisk(MyObject);
	
			if (ObjectThumbnail)
			{
				IImageWrapperModule& ImageWrapperModule = FModuleManager::Get().LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));
				TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
				ImageWrapper->SetRaw(
					ObjectThumbnail->GetUncompressedImageData().GetData(),
					ObjectThumbnail->GetUncompressedImageData().Num(),
					ObjectThumbnail->GetImageWidth(),
					ObjectThumbnail->GetImageHeight(),
					ERGBFormat::BGRA,
					8);
				if (ImageWrapper)
				{
					const TArray64<uint8>& CompressedByteArray = ImageWrapper->GetCompressed();
					FFileHelper::SaveArrayToFile(CompressedByteArray, *(BaseExportPath + ObjectPath.GetAssetPathString() + ".png"));
				}
			}
		}
	}
}
