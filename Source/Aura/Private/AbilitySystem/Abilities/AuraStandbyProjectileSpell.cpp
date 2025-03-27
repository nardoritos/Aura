// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "AbilitySystem/Abilities/AuraStandbyProjectileSpell.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraStandbyProjectile.h"
#include "Character/AuraCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"

TArray<AAuraStandbyProjectile*> UAuraStandbyProjectileSpell::SpawnProjectiles(AActor* AttachToActor)
{
	TArray<AAuraStandbyProjectile*> SpawnedProjectiles;
	
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if(!bIsServer) return SpawnedProjectiles;

 	AAuraCharacter* OwnerCharacter = Cast<AAuraCharacter>(GetAvatarActorFromActorInfo());
	if (!IsValid(OwnerCharacter)) return SpawnedProjectiles;
	
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector SpawnLocation = (AttachToActor ? AttachToActor->GetActorLocation() : GetAvatarActorFromActorInfo()->GetActorLocation()) + FVector(0,0,50); 

	const int32 NumProjectilesToSpawn = bDrawDebug ? DebugProjectiles : GetAbilityLevel();
	TArray<FRotator> Rotators = UAuraAbilitySystemLibrary::EvenlySpacedRotators(
		FVector::UpVector, Forward, 180.0, NumProjectilesToSpawn);	

	for (FRotator& Rot : Rotators)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SpawnLocation);
		SpawnTransform.SetRotation(Rot.Quaternion());

		if (bDrawDebug)
		{
			DrawDebugDirectionalArrow(
				GetWorld(),
				SpawnTransform.GetLocation(),
				SpawnTransform.GetLocation(),
				12,
				FColor::White,
				false,
				5.f,
				0,
				5.f
				);
			
		}
		AAuraStandbyProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraStandbyProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		if (AttachToActor)
		{
			Projectile->AttachToActor(AttachToActor, FAttachmentTransformRules::KeepWorldTransform);
		}
		
		Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
		Projectile->BindToPlayer(OwnerCharacter);
		Projectile->ProjectileMovement->bIsHomingProjectile = true;
		Projectile->FinishSpawning(SpawnTransform);

		SpawnedProjectiles.AddUnique(Projectile);
	}

	return SpawnedProjectiles;
}
