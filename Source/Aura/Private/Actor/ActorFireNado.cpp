// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "Actor/ActorFireNado.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AActorFireNado::AActorFireNado()
{
	PrimaryActorTick.bCanEverTick = true;
	AttractionPointComponent = CreateDefaultSubobject<USceneComponent>(TEXT("AttractionPoint"));
	AttractionPointComponent->SetupAttachment(Box);
	
}

void AActorFireNado::BeginPlay()
{
	Super::BeginPlay();
	
}

void AActorFireNado::Destroyed()
{
	for (auto Enemy : EnemiesToPull)
	{
		if (IsValid(Enemy))
		{
			Enemy->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
	}
	
	Super::Destroyed();
	
}

void AActorFireNado::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!IsValidOverlap(OtherActor)) return;
	if (!bHit) OnHit();	
	
	if(HasAuthority())
	{
		if (AAuraEnemy* Enemy = Cast<AAuraEnemy>(OtherActor))
		{
			EnemiesToPull.AddUnique(Enemy);
			Enemy->GetCharacterMovement()->DisableMovement();
			
			if (bApplyDamageAutomaticallyOnCollision)
			{
				ApplyDamageToTarget(OtherActor);
			}
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

void AActorFireNado::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator RotatesTowardAxis = FRotator();
	RotatesTowardAxis.Yaw = 1.0;
	AddActorLocalRotation(RotatesTowardAxis);

	PullEnemiesToAttractionPoint(DeltaTime);
}

void AActorFireNado::ApplyDamageToPulledTargets()
{
	for (auto Enemy : EnemiesToPull)
	{
		if (IsValid(Enemy))
		{
			ApplyDamageToTarget(Enemy);
		}
	}
}

void AActorFireNado::PullEnemiesToAttractionPoint(const float DeltaTime)
{
	if (!EnemiesToPull.IsEmpty())
	{
		TArray<FVector> Directions = UAuraAbilitySystemLibrary::EvenlyRotatedVectors(
			AttractionPointComponent->GetForwardVector(),
			FVector::UpVector,
			360.f,
			EnemiesToPull.Num() + 1);
		
		for (int i = 0; i < EnemiesToPull.Num(); i++)
		{
			AAuraEnemy* Enemy = EnemiesToPull[i];
			if (Directions.IsValidIndex(i))
			{
				FVector Direction = Directions[i];
				if (IsValid(Enemy))
				{
					
					FVector AttractionPoint = (Direction * PointDistance) + AttractionPointComponent->GetComponentLocation();
				
					Enemy->SetActorLocation(FMath::VInterpConstantTo(
						Enemy->GetActorLocation(),
						AttractionPoint,
						DeltaTime,
						PullForce
					));
				}
			}
		}
	}
}

