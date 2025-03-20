// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AuraProjectileBase.h"
#include "AuraProjectileBox.generated.h"

class UBoxComponent;

UCLASS()
class AURA_API AAuraProjectileBox : public AAuraProjectileBase
{
	GENERATED_BODY()

public:	
	AAuraProjectileBox();

protected:

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ApplyDamageToTarget(AActor* OtherActor);

	UFUNCTION()
	virtual void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> Box;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	bool bApplyDamageAutomaticallyOnCollision = false;
	
};
