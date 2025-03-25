// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "Actor/AuraProjectileSphere.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/Aura.h"
#include "Components/SphereComponent.h"

AAuraProjectileSphere::AAuraProjectileSphere()
{
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_PROJECTILE);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AAuraProjectileSphere::BeginPlay()
{
	Super::BeginPlay();
	if (bShouldBindToMainSphere)
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectileSphere::OnSphereOverlap);
}

void AAuraProjectileSphere::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!IsValidOverlap(OtherActor)) return;
	if (!bHit) OnHit();	
	
	if(HasAuthority())
	{
		if(UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			if (bShouldCauseDamage)
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
			else
			{
				HealingEffectParams.TargetAbilitySystemComponent = TargetASC;
				UAuraAbilitySystemLibrary::ApplyRegenEffect(HealingEffectParams);
			}
		}
		Destroy();
	}
	else
	{
		bHit = true;
	}
}