// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MinimapTrackerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AURA_API UMinimapTrackerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UMinimapTrackerComponent();
	
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, Category = "MinimapTracker")
	void RegisterComponentToReceiver();
	
	UFUNCTION(BlueprintCallable, Category = "MinimapTracker")
	FVector2D GetPositionInScreen(const float ScaledSize, const float LimitToClamp = 100.f) const;

	UFUNCTION(BlueprintCallable, Category = "MinimapTracker")
	UTexture* GetSprite() {  return Sprite; }
	UFUNCTION(BlueprintCallable, Category = "MinimapTracker")
	void SetSprite(UTexture* InTexture);

	UFUNCTION(BlueprintCallable, Category = "MinimapTracker")
	bool GetSpriteDirty() const { return bSpriteDirty; }
	UFUNCTION(BlueprintCallable, Category = "MinimapTracker")
	void MarkSpriteDirty(bool bInDirty);

	UFUNCTION(BlueprintCallable, Category = "MinimapTracker")
	bool ShouldRenderOutsideBounds() const { return bShouldRenderOutsideBounds; }
	UFUNCTION(BlueprintCallable, Category = "MinimapTracker")
	void SetShouldRenderOutsideBounds(bool bInShouldRenderOutsideBounds);
	
	UFUNCTION(BlueprintCallable, Category = "MinimapTracker")
	bool IsInRange(const float Range) const;
	
	void SetReferenceActor(AActor* InReferenceActor);
	AActor* GetReferenceActor() const;
	
protected:

	bool bSpriteDirty = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MinimapTracker")
	TObjectPtr<UTexture> Sprite;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MinimapTracker")
	bool bShouldRenderOutsideBounds = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MinimapTracker")
	bool bEnabled = true;

	UPROPERTY()
	TObjectPtr<AActor> ReferenceActor = nullptr;
	
};
