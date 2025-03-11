// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "Actor/AuraEnemySpawnPoint.h"

#include "Character/AuraEnemy.h"
#include "WorldPartition/Cook/WorldPartitionCookPackage.h"

void AAuraEnemySpawnPoint::SpawnEnemy()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AAuraEnemy* Enemy = GetWorld()->SpawnActorDeferred<AAuraEnemy>(EnemyClass, GetActorTransform());
	Enemy->SetLevel(EnemyLevel);
	Enemy->SetCharacterClass(CharacterClass);
	Enemy->FinishSpawning(GetActorTransform());
	Enemy->SpawnDefaultController();
	
}
