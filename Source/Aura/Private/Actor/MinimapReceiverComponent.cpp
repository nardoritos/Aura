// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "Actor/MinimapReceiverComponent.h"

#include "Blueprint/UserWidget.h"

void UMinimapReceiverComponent::AddTracker(UMinimapTrackerComponent* NewTracker)
{
	TrackedObjects.Add(NewTracker);
}

void UMinimapReceiverComponent::RemoveTracker(UMinimapTrackerComponent* TrackerToRemove)
{
	if (TrackedObjects.Contains(TrackerToRemove))
	{
		TrackedObjects.Find(TrackerToRemove)->Get()->RemoveFromParent();

		TrackedObjects.Remove(TrackerToRemove);
	}
}

void UMinimapReceiverComponent::RemoveTrackerByWidget(UUserWidget* WidgetToRemove)
{
	UMinimapTrackerComponent* KeyToRemove = nullptr;
	for (auto Pair : TrackedObjects)
	{
		if (Pair.Value.Get() == WidgetToRemove)
		{
			KeyToRemove = Pair.Key.Get();
			break;
		}
	}
	
	TrackedObjects.Remove(KeyToRemove);
	WidgetToRemove->RemoveFromParent();
}

void UMinimapReceiverComponent::AddWidgetToTracker(UMinimapTrackerComponent* TrackerToAddWidgetTo, UUserWidget* TrackerWidget)
{
	TrackedObjects.Add(TrackerToAddWidgetTo, TrackerWidget);
	
}

TArray<UMinimapTrackerComponent*> UMinimapReceiverComponent::GetTrackers()
{
	TArray<UMinimapTrackerComponent*> Trackers;
	for (auto Pair  : TrackedObjects)
	{
		Trackers.Add(Pair.Key.Get());
	}
	return Trackers;
}

bool UMinimapReceiverComponent::TrackerHasWidget(UMinimapTrackerComponent* TrackerToCheck) const
{
	if (TrackedObjects.Contains(TrackerToCheck))
	{
		auto ReturnValue = TrackedObjects.Find(TrackerToCheck)->Get();
		return TrackedObjects.Find(TrackerToCheck)->Get() != nullptr;
	}

	return false;
}

void UMinimapReceiverComponent::SetMinimapInstance(UMaterialInstance* InMinimapInstance)
{
	MinimapInstance = InMinimapInstance;
}
 