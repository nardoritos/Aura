// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actor/AuraProjectileSphere.h"
#include "Components/TimelineComponent.h"
#include "AuraFireBall.generated.h"

class UTimelineComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraFireBall : public AAuraProjectileSphere
{
	GENERATED_BODY()

protected:

	AAuraFireBall();
	virtual void BeginPlay() override;
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnHit() override;
	
	// Timelines
	void SetupTimer();

	FVector StartLocation = FVector::ZeroVector;
	FVector Apex = FVector::ZeroVector;
	
	UPROPERTY(EditDefaultsOnly, Category = "Timelines")
	float Distance = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Timelines")
	float ExplodingDistance = 0.f;

	// Outgoing Timeline

	UPROPERTY()
	TObjectPtr<UTimelineComponent> OutgoingTimeline;

	UPROPERTY(EditDefaultsOnly, Category = "Timelines")
	TObjectPtr<UCurveFloat> OutgoingCurve;

	FOnTimelineFloat UpdateOutgoing{};
	FOnTimelineEvent FinishedOutgoing{};

	UFUNCTION()
	void OutgoingUpdate(float Value);

	UFUNCTION()
	void OutgoingFinished() const;

	// Returning Timeline
	
	UPROPERTY()
	TObjectPtr<UTimelineComponent> ReturningTimeline;

	UPROPERTY(EditDefaultsOnly, Category = "Timelines")
	TObjectPtr<UCurveFloat> ReturnCurve;

	FOnTimelineFloat UpdateReturn{};

	UFUNCTION()
	void ReturnUpdate(float Value);

	// Damage Effect Params
	
	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	float RadialDamageInnerRadius = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	float RadialDamageOuterRadius = 300.f;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	float KnockbackMagnitude = 800.f;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	float DeathImpulseMagnitude = 600.f;

public:
	
	UPROPERTY(BlueprintReadWrite)
    FDamageEffectParams ExplosionDamageParams;
    	
};
