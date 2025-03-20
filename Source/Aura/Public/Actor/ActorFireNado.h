// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AuraProjectileBox.h"
#include "Character/AuraEnemy.h"
#include "ActorFireNado.generated.h"

UCLASS()
class AURA_API AActorFireNado : public AAuraProjectileBox
{
	GENERATED_BODY()

public:
	AActorFireNado();

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	virtual void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

public:
	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)	
	void ApplyDamageToPulledTargets();

private:
	
	void PullEnemiesToAttractionPoint(const float DeltaTime);
	
	TArray<TObjectPtr<AAuraEnemy>> EnemiesToPull;

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> AttractionPointComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float PullForce = 1500.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float PointDistance = 100.f;
};
