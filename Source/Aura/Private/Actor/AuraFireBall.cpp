// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "Actor/AuraFireBall.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "GameplayCueManager.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Components/AudioComponent.h"
#include "Kismet/KismetMathLibrary.h"

AAuraFireBall::AAuraFireBall()
{
	OutgoingTimeline = CreateDefaultSubobject<UTimelineComponent>("OutgoingTimeline");
	UpdateOutgoing.BindUFunction(this, FName("OutgoingUpdate"));
	FinishedOutgoing.BindUFunction(this, FName("OutgoingFinished"));

	ReturningTimeline = CreateDefaultSubobject<UTimelineComponent>("ReturningTimeline");
	UpdateReturn.BindUFunction(this, FName("ReturnUpdate"));
}

void AAuraFireBall::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SetupTimer();
		OutgoingTimeline->Play();
	}
}

void AAuraFireBall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValidOverlap(OtherActor)) return;

	if(HasAuthority())
	{
		if(UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			DamageEffectParams.DeathImpulse = DeathImpulse;
			
			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
	}
}

void AAuraFireBall::OnHit()
{
	if (GetOwner())
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = GetActorLocation();
		
		UGameplayCueManager::ExecuteGameplayCue_NonReplicated(
			GetOwner(),
			FAuraGameplayTags::Get().GameplayCue_FireBlast,
			CueParams);
	}
	
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}
	bHit = true;
}

void AAuraFireBall::SetupTimer()
{
	if (OutgoingCurve && ReturnCurve)
	{
		OutgoingTimeline->AddInterpFloat(OutgoingCurve, UpdateOutgoing, FName("Alpha"));
		OutgoingTimeline->SetTimelineFinishedFunc(FinishedOutgoing);
		OutgoingTimeline->SetLooping(false);
		OutgoingTimeline->SetIgnoreTimeDilation(true);

		ReturningTimeline->AddInterpFloat(ReturnCurve, UpdateReturn, FName("Alpha"));
		ReturningTimeline->SetLooping(false);
		ReturningTimeline->SetIgnoreTimeDilation(true);
	}
	StartLocation = GetActorLocation();
	Apex = StartLocation + (GetActorForwardVector() * Distance);
}

void AAuraFireBall::OutgoingUpdate(float Value)
{
	const FVector LerpLocation = UKismetMathLibrary::VLerp(StartLocation, Apex, Value);
	SetActorLocation(LerpLocation);
}

void AAuraFireBall::OutgoingFinished() const
{
	ReturningTimeline->Play();
}

void AAuraFireBall::ReturnUpdate(float Value)
{
	const APawn* ReturnToActor = GetInstigator(); 
	if (IsValid(ReturnToActor))
	{
		const FVector LerpLocation = UKismetMathLibrary::VLerp(Apex, ReturnToActor->GetActorLocation(), Value);
		SetActorLocation(LerpLocation);
		const float DistanceToReturn = GetDistanceTo(ReturnToActor);
		if (DistanceToReturn <= ExplodingDistance)
		{
			OnHit();
			
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(GetInstigator());

			TArray<AActor*> OverlappingActors;
			
			UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
				GetInstigator(),
				OverlappingActors,
				ActorsToIgnore,
				RadialDamageOuterRadius,
				GetActorLocation());

			UAuraAbilitySystemLibrary::SetIsRadialDamageEffectParam(
				ExplosionDamageParams,
				true,
				RadialDamageInnerRadius,
				RadialDamageOuterRadius,
				GetActorLocation());

			for (AActor* TargetActor : OverlappingActors)
			{
				if (UAuraAbilitySystemLibrary::IsNotFriend(GetInstigator(), TargetActor))
				{
					UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
					UAuraAbilitySystemLibrary::SetTargetEffectParamsASC(ExplosionDamageParams, TargetASC);
				
					const FVector DirectionToOverlappingActor = TargetActor->GetActorLocation() - GetActorLocation();
					FRotator KnockbackRotation = DirectionToOverlappingActor.Rotation();
					KnockbackRotation.Pitch = 45.f;
					const FVector KnockbackDirection = KnockbackRotation.Quaternion().GetForwardVector();
				
					UAuraAbilitySystemLibrary::SetKnockbackDirection(ExplosionDamageParams, KnockbackDirection, KnockbackMagnitude);
				
					UAuraAbilitySystemLibrary::SetDeathImpulseDirection(ExplosionDamageParams, DirectionToOverlappingActor, DeathImpulseMagnitude);

					UAuraAbilitySystemLibrary::ApplyDamageEffect(ExplosionDamageParams);
				}
			}
			
			Destroy();
		}
	}
	else
	{
		OnHit();
		Destroy();
	}
}
