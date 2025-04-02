// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MinimapReceiverComponent.generated.h"


class UMinimapTrackerComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AURA_API UMinimapReceiverComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "MinimapReceiver")
	void AddTracker(UMinimapTrackerComponent* NewTracker);

	UFUNCTION(BlueprintCallable, Category = "MinimapReceiver")
	void RemoveTracker(UMinimapTrackerComponent* TrackerToRemove);

	UFUNCTION(BlueprintCallable, Category = "MinimapReceiver")
	void RemoveTrackerByWidget(UUserWidget* WidgetToRemove);
	
	UFUNCTION(BlueprintCallable, Category = "MinimapReceiver")
	void AddWidgetToTracker(UMinimapTrackerComponent* TrackerToAddWidgetTo, UUserWidget* TrackerWidget);

	UFUNCTION(BlueprintCallable, Category = "MinimapReceiver")
	TArray<UMinimapTrackerComponent*> GetTrackers();

	UFUNCTION(BlueprintCallable, Category = "MinimapReceiver")
	bool TrackerHasWidget(UMinimapTrackerComponent* TrackerToCheck) const;

	UFUNCTION(BlueprintCallable, Category = "MinimapReceiver")
	void SetMinimapInstance(UMaterialInstance* InMinimapInstance);

	UFUNCTION(BlueprintCallable, Category = "MinimapReceiver")
	UMaterialInstance* GetMinimapInstance() { return MinimapInstance; }

protected:

	UPROPERTY()
	TMap<TObjectPtr<UMinimapTrackerComponent>, TObjectPtr<UUserWidget>> TrackedObjects;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInstance> MinimapInstance;
};

