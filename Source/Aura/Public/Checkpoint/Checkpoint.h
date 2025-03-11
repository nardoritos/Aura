// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Aura/Aura.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/HighlightInterface.h"
#include "Interaction/SaveInterface.h"
#include "Checkpoint.generated.h"

class USphereComponent;
/**
 * 
 */
UCLASS()
class AURA_API ACheckpoint : public APlayerStart, public ISaveInterface, public IHighlightInterface
{
	GENERATED_BODY()

public:

	ACheckpoint(const FObjectInitializer& ObjectInitializer);

	/* Save Interface */
	virtual bool ShouldLoadTransform_Implementation() override { return false; }
	virtual void LoadActor_Implementation() override;
	/* End Save Interface*/
	
	UPROPERTY(BlueprintReadWrite, SaveGame)
	bool bReached = false;

	UPROPERTY(EditAnywhere)
	bool bBindOverlapCallback = true;
protected:

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void BeginPlay() override;
	
	/* Highlight Interface */
	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;
	virtual bool SetMoveToLocation_Implementation(FVector& OutDestination) override;
	/* End Highlight Interface */

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> MoveToComponent;
	
	UPROPERTY(EditDefaultsOnly)
	int32 CustomDepthStencilOverride = CUSTOM_DEPTH_TAN;
	
	UFUNCTION(BlueprintImplementableEvent)
	void CheckpointReached(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UFUNCTION(BlueprintCallable)
	void HandleGlowEffects();
	
	UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> CheckpointMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;
};
