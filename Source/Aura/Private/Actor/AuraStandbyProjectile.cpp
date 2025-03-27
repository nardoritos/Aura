// Copyright (c) 2024, Nardoritos. All rights reserved.

#include "Actor/AuraStandbyProjectile.h"
#include "Character/AuraCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"


AAuraStandbyProjectile::AAuraStandbyProjectile()
{
	ProjectileMovement->MaxSpeed = 0.f;
}

void AAuraStandbyProjectile::Destroyed()
{
	if (IsValid(OwnerCharacter))
	{
		OwnerCharacter->OnProjectileDetectedEnemy.RemoveAll(this);
	}

	TimerHandle_IncreaseSpeed.Invalidate();
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

	OnProjectileHit.Broadcast(this, GetActorTransform());
	
	Super::Destroyed();
}

void AAuraStandbyProjectile::BindToPlayer(AAuraCharacter* InCharacter)
{
	if (!IsValid(InCharacter)) return;

	OwnerCharacter = InCharacter;
	OwnerCharacter->OnProjectileDetectedEnemy.AddUObject(this, &AAuraStandbyProjectile::UpdateHomingTarget);
	
}

void AAuraStandbyProjectile::AnimationFinished()
{
	bHasFinishedAnimation = true;
	if (OwnerCharacter)
	{
		OwnerCharacter->CheckForCurrentProjectileDetectionOverlaps();
	}
}

void AAuraStandbyProjectile::UpdateHomingTarget(AActor* HomingTarget)
{
	if (!bHasFinishedAnimation) return;
	if (IsValid(OwnerCharacter))
	{
		OwnerCharacter->OnProjectileDetectedEnemy.RemoveAll(this);
	}
	if (!IsValid(ProjectileMovement)) return;
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetHomingTarget(HomingTarget, GetActorLocation());

	ProjectileMovement->InitialSpeed = 500.f;
	ProjectileMovement->MaxSpeed = 1000.f;
	
	ProjectileMovement->HomingAccelerationMagnitude = ProjectileMovement->InitialSpeed;
	
	FTimerDelegate Delegate;
	Delegate.BindLambda([this]()
	{
		ProjectileMovement->InitialSpeed *= 1.1f;
		ProjectileMovement->HomingAccelerationMagnitude = ProjectileMovement->InitialSpeed;
	});
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_IncreaseSpeed, Delegate, 0.2f, true);

}

