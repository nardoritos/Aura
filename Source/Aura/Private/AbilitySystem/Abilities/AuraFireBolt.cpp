// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "AbilitySystem/Abilities/AuraFireBolt.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraProjectileSphere.h"

void UAuraFireBolt::SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag,
                                     const bool bOverridePitch, const float PitchOverride, AActor* HomingTarget)
{

	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if(!bIsServer) return;

	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(
		GetAvatarActorFromActorInfo(),
		SocketTag);
	
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();

	if (bOverridePitch) Rotation.Pitch = PitchOverride;

	const FVector Forward = Rotation.Vector();

	const int32 NumProjectilesToSpawn = MaxNumProjectiles.GetValueAtLevel(GetAbilityLevel());
	TArray<FRotator> Rotators = UAuraAbilitySystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, ProjectileSpread, NumProjectilesToSpawn);	

	for (FRotator& Rot : Rotators)
	{
		FTransform  SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rot.Quaternion());

		AAuraProjectileSphere* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectileSphere>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();

		Projectile->SetHomingTarget(HomingTarget, ProjectileTargetLocation);

		Projectile->ProjectileMovement->HomingAccelerationMagnitude = FMath::RandRange(HomingAccelerationMin, HomingAccelerationMax);
		Projectile->ProjectileMovement->bIsHomingProjectile = bLaunchHomingProjectiles;
		
		Projectile->FinishSpawning(SpawnTransform);
	}
	
	
	
}
