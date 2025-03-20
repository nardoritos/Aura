// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/SaveInterface.h"
#include "AuraEnemySpawnVolume.generated.h"

class AAuraEnemy;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FChangedEnemiesAliveSignature, AAuraEnemySpawnVolume*, EnemySpawnVolume, bool, bHasEnemiesAlive);

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

public:
	
	bool HasEnemiesAlive();
	FChangedEnemiesAliveSignature& GetOnSpawnVolumeStateChangedDelegate();
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemies")
	TArray<AAuraEnemySpawnPoint*> SpawnPoints;

	TArray<TObjectPtr<AAuraEnemy>> TrackedEnemies;
	
	FChangedEnemiesAliveSignature OnSpawnVolumeStateChanged;

	UFUNCTION()
	void SpawnedEnemyHasDied(AActor* DeadActor);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemies")
	bool bHasAliveEnemies = false;
	bool bHasRegisteredEnemyTracking = false;
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> Box;
};
