// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Checkpoint/Checkpoint.h"
#include "MapEntrance.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AMapEntrance : public ACheckpoint
{
	GENERATED_BODY()

public:

	AMapEntrance(const FObjectInitializer& ObjectInitializer);
	
	/* Save Interface*/
	virtual void LoadActor_Implementation() override;
	/* End Save Interface */
	
	/* Highlight Interface */
	virtual void HighlightActor_Implementation() override;
	/* End Highlight Interface */

	UPROPERTY(EditAnywhere, Category = "Checkpoint")
	TSoftObjectPtr<UWorld> DestinationMap;

	UPROPERTY(EditAnywhere, Category = "Checkpoint")
	bool bIsGameOver = false;
	
	UPROPERTY(EditAnywhere, Category = "Checkpoint")
	FName DestinationPlayerStartTag = FName();

protected:
	
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	
};
