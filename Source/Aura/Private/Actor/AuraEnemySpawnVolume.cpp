// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "Actor/AuraEnemySpawnVolume.h"

#include "Actor/AuraEnemySpawnPoint.h"
#include "Character/AuraEnemy.h"
#include "Components/BoxComponent.h"
#include "Interaction/PlayerInterface.h"

AAuraEnemySpawnVolume::AAuraEnemySpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Box = CreateDefaultSubobject<UBoxComponent>("Box");
	SetRootComponent(Box);
	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Box->SetCollisionObjectType(ECC_WorldStatic);
	Box->SetCollisionResponseToAllChannels(ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

}

void AAuraEnemySpawnVolume::LoadActor_Implementation()
{
	if (bReached)
	{
		bHasAliveEnemies = false;
		OnSpawnVolumeStateChanged.Broadcast(this, bHasAliveEnemies);
		Destroy();
	}
	else
	{
		bHasAliveEnemies = true;
		OnSpawnVolumeStateChanged.Broadcast(this, bHasAliveEnemies);
		bHasRegisteredEnemyTracking = true;
	}
}

bool AAuraEnemySpawnVolume::HasEnemiesAlive()
{
	return bHasAliveEnemies;
}

FChangedEnemiesAliveSignature& AAuraEnemySpawnVolume::GetOnSpawnVolumeStateChangedDelegate()
{
	return OnSpawnVolumeStateChanged;
}

void AAuraEnemySpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	Box->OnComponentBeginOverlap.AddDynamic(this, &AAuraEnemySpawnVolume::OnBoxOverlap);

	if (SpawnPoints.IsEmpty())
	{
		bHasAliveEnemies = false;
		OnSpawnVolumeStateChanged.Broadcast(this, bHasAliveEnemies);
		Destroy();
		return;
	}
	if (!bHasRegisteredEnemyTracking)
	{
		bHasRegisteredEnemyTracking = true;
		bHasAliveEnemies = true;
		OnSpawnVolumeStateChanged.Broadcast(this, bHasAliveEnemies);
	}
	
}

void AAuraEnemySpawnVolume::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->Implements<UPlayerInterface>()) return;
	
	bReached = true;

	for (AAuraEnemySpawnPoint* Point : SpawnPoints)
	{
		if (IsValid(Point))
		{
			AAuraEnemy* SpawnedEnemy = Point->SpawnEnemy();
			SpawnedEnemy->GetOnDeathDelegate().AddDynamic(this, &AAuraEnemySpawnVolume::SpawnedEnemyHasDied);
			TrackedEnemies.AddUnique(SpawnedEnemy);
		}
	}
	Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAuraEnemySpawnVolume::SpawnedEnemyHasDied(AActor* DeadActor)
{
	if (AAuraEnemy* DeadEnemy = Cast<AAuraEnemy>(DeadActor))
	{
		if (TrackedEnemies.Contains(DeadEnemy))
		{
			TrackedEnemies.Remove(DeadEnemy);
		}
	}
	bHasAliveEnemies = !TrackedEnemies.IsEmpty();
	OnSpawnVolumeStateChanged.Broadcast(this, bHasAliveEnemies);
}




