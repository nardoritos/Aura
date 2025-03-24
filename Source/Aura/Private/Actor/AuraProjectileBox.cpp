// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "Actor/AuraProjectileBox.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/Aura.h"
#include "Components/BoxComponent.h"

AAuraProjectileBox::AAuraProjectileBox()
{
	Box = CreateDefaultSubobject<UBoxComponent>("Box");
	SetRootComponent(Box);
	Box->SetCollisionObjectType(ECC_PROJECTILE);
	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Box->SetCollisionResponseToAllChannels(ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Box->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}


void AAuraProjectileBox::BeginPlay()
{
	Super::BeginPlay();
	Box->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectileBox::OnBoxOverlap);
}

void AAuraProjectileBox::ApplyDamageToTarget(AActor* OtherActor)
{
	if(UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
	{
		const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
		DamageEffectParams.DeathImpulse = DeathImpulse;

		const bool bKnockback = FMath::RandRange(1,100) < DamageEffectParams.KnockbackChance;
		if (bKnockback)
		{
			FRotator Rotation = GetActorRotation();
			Rotation.Pitch = 45.f;
				
			const FVector KnockbackDirection = Rotation.Vector();
			const FVector KnockbackForce = KnockbackDirection * DamageEffectParams.KnockbackForceMagnitude;
			DamageEffectParams.KnockbackForce = KnockbackForce;
		}
			
		DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
		UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
	}
}

void AAuraProjectileBox::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!IsValidOverlap(OtherActor)) return;
	if (!bHit) OnHit();	
	
	if(HasAuthority())
	{
		if (bApplyDamageAutomaticallyOnCollision)
		{
			ApplyDamageToTarget(OtherActor);
		}
		if (bDestroyOnHit)
		{
			Destroy();
		}
	}
	else
	{
		bHit = true;
	}
}
