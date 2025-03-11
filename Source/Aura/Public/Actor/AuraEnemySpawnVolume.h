// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/SaveInterface.h"
#include "AuraEnemySpawnVolume.generated.h"

class AAuraEnemySpawnPoint;
class UBoxComponent;

UCLASS()
class AURA_API AAuraEnemySpawnVolume : public AActor, public ISaveInterface
{
	GENERATED_BODY()
	
public:	
	AAuraEnemySpawnVolume();

	/* Save Interface */
	virtual void LoadActor_Implementation() override;
	/* End Save Interface */

	UPROPERTY(BlueprintReadonly, SaveGame)
	bool bReached = false;
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<AAuraEnemySpawnPoint*> SpawnPoints;
	
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> Box;
};
