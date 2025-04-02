// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "Actor/MinimapTrackerComponent.h"

#include "Actor/MinimapReceiverComponent.h"
#include "GameFramework/Character.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UMinimapTrackerComponent::UMinimapTrackerComponent()
{
	bAutoActivate = true;
}

void UMinimapTrackerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (bEnabled)
	{
		RegisterComponentToReceiver();
	}
}

void UMinimapTrackerComponent::RegisterComponentToReceiver()
{
	if(const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		ACharacter* PlayerCharacter = PlayerController->GetCharacter();
		if(IsValid(PlayerCharacter) && PlayerCharacter->Implements<UPlayerInterface>())
		{
			SetReferenceActor(PlayerCharacter);
			IPlayerInterface::Execute_GetMinimapComponent(PlayerCharacter)->AddTracker(this);
		}
	}
}

FVector2D UMinimapTrackerComponent::GetPositionInScreen(const float ScaledSize, const float LimitToClamp) const
{
	const FVector ReferenceLocation = ReferenceActor->GetActorLocation();
	
	const FVector OwnerLocation = GetOwner()->GetActorLocation();
	
	const float ScaledX = (ReferenceLocation.X - OwnerLocation.X) / ScaledSize;
	const float ScaledY = (ReferenceLocation.Y - OwnerLocation.Y) / ScaledSize;
	
	const FVector2D ScaledLocation = FVector2D(ScaledX, ScaledY);
	const float LocationRadius = UKismetMathLibrary::VSize2D(ScaledLocation);
	
	// Find Angle in the Minimap
	const float AngleDegrees = UKismetMathLibrary::DegAtan2(
		0.f - ScaledLocation.Y,
		0.f - ScaledLocation.X);

	// Find Coordinates in the Minimap
	const float ClampedRadius = UKismetMathLibrary::FClamp(LocationRadius, 0.f, LimitToClamp);
	const FVector2D Coords = FVector2D(
		ClampedRadius * UKismetMathLibrary::DegSin(AngleDegrees),
		ClampedRadius * UKismetMathLibrary::DegCos(AngleDegrees) * -1.f);
	
	return Coords;
}

void UMinimapTrackerComponent::SetSprite(UTexture* InTexture)
{
	Sprite = InTexture;
	MarkSpriteDirty(true);
}

void UMinimapTrackerComponent::MarkSpriteDirty(bool bInDirty)
{
	bSpriteDirty = bInDirty;
}

void UMinimapTrackerComponent::SetShouldRenderOutsideBounds(bool bInShouldRenderOutsideBounds)
{
	bShouldRenderOutsideBounds = bInShouldRenderOutsideBounds;
}

bool UMinimapTrackerComponent::IsInRange(const float Range) const
{
	return GetOwner()->GetHorizontalDistanceTo(ReferenceActor) <= Range;
}

void UMinimapTrackerComponent::SetReferenceActor(AActor* InReferenceActor)
{
	ReferenceActor = InReferenceActor;
}

AActor* UMinimapTrackerComponent::GetReferenceActor() const
{
	return ReferenceActor;
}

