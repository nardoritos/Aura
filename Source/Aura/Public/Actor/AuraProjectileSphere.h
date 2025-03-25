// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AuraProjectileBase.h"
#include "GameFramework/Actor.h"
#include "AuraProjectileSphere.generated.h"


class USphereComponent;


UCLASS()
class AURA_API AAuraProjectileSphere : public AAuraProjectileBase
{
	GENERATED_BODY()
	
public:	
	AAuraProjectileSphere();

protected:

	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(EditDefaultsOnly)
	bool bShouldBindToMainSphere = true;
	
};
