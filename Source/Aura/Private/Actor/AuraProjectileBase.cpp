// Copyright (c) 2024, Nardoritos. All rights reserved.

#include "Actor/AuraProjectileBase.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Components/AudioComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"

AAuraProjectileBase::AAuraProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	AActor::SetReplicateMovement(true);
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

void AAuraProjectileBase::SetHomingTarget(AActor* HomingTargetActor, FVector FallbackLocation)
{
	if (HomingTargetActor && HomingTargetActor->Implements<UCombatInterface>())
	{
		ProjectileMovement->HomingTargetComponent = HomingTargetActor->GetRootComponent();
	}
	else
	{
		HomingTargetSceneComponent = NewObject<USceneComponent>(USceneComponent::StaticClass());
		HomingTargetSceneComponent->SetWorldLocation(FallbackLocation);
		ProjectileMovement->HomingTargetComponent = HomingTargetSceneComponent;
	}
}

void AAuraProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(LifeSpan);
	

	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
	
}

void AAuraProjectileBase::Destroyed()
{
	StopSound();
	if(!bHit && !HasAuthority()) OnHit();
	Super::Destroyed();
}

void AAuraProjectileBase::StopSound()
{
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}
}



bool AAuraProjectileBase::IsValidOverlap(AActor* OtherActor)
{
	if (bShouldCauseDamage)
	{
		if (!IsValid(DamageEffectParams.SourceAbilitySystemComponent)) return false;
		
		AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
		if (SourceAvatarActor == OtherActor) return false;

		if (!UAuraAbilitySystemLibrary::IsNotFriend(SourceAvatarActor,  OtherActor)) return false;
		return true;
	}
	if (!IsValid(HealingEffectParams.TargetAbilitySystemComponent)) return false;

	AActor* TargetAvatarActor = HealingEffectParams.TargetAbilitySystemComponent->GetAvatarActor();
	if (TargetAvatarActor == GetInstigator()) return false;

	if (!UAuraAbilitySystemLibrary::IsNotFriend(GetInstigator(),  OtherActor)) return false;
	
	return true;
}

void AAuraProjectileBase::OnHit()
{
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());

	UE_LOG(LogTemp, Warning, TEXT("Cosmetic Effect played"))
	
	StopSound();

	bHit = true;
}
