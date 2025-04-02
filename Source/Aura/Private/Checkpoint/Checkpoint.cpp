// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "Checkpoint/Checkpoint.h"

#include "Actor/AuraEnemySpawnVolume.h"
#include "Actor/MinimapTrackerComponent.h"
#include "Components/SphereComponent.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"

ACheckpoint::ACheckpoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	CheckpointMesh = CreateDefaultSubobject<UStaticMeshComponent>("CheckpointMesh");
	CheckpointMesh->SetupAttachment(GetRootComponent());
	CheckpointMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CheckpointMesh->SetCollisionResponseToAllChannels(ECR_Block);
	CheckpointMesh->SetCustomDepthStencilValue(CustomDepthStencilOverride);
	CheckpointMesh->MarkRenderStateDirty();
	
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(CheckpointMesh);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	MoveToComponent = CreateDefaultSubobject<USceneComponent>("MoveToComponent");
	MoveToComponent->SetupAttachment(GetRootComponent());

	MinimapTrackerComponent = CreateDefaultSubobject<UMinimapTrackerComponent>("MinimapTrackerComponent");
	
}

void ACheckpoint::LoadActor_Implementation()
{
	if (bReached)
	{
		HandleGlowEffects();
		MinimapTrackerComponent->SetSprite(UsedSprite);
	}
	else
	{
		if (!bHasRegisteredSpawnVolumes)
		{
			RegisterSpawnVolumes();
			
		}
		CheckEnabledState();
	}
}

void ACheckpoint::HighlightActor_Implementation()
{
	if (!bReached && bIsCheckpointEnabled)
	{
		CheckpointMesh->SetRenderCustomDepth(true);
	}
}

void ACheckpoint::UnHighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(false);
}

bool ACheckpoint::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	OutDestination = MoveToComponent->GetComponentLocation();
	return true;
}

void ACheckpoint::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UPlayerInterface>())
	{
		bReached = true;
		MinimapTrackerComponent->SetSprite(UsedSprite);
		
		if(AAuraGameModeBase* AuraGM = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			UWorld* World= GetWorld();
			FString MapName = World->GetMapName();
			MapName.RemoveFromStart(World->StreamingLevelsPrefix);
			
			AuraGM->SaveWorldState(World, MapName);
		}

		
		IPlayerInterface::Execute_SaveProgress(OtherActor, PlayerStartTag);
		HandleGlowEffects();
	}
}

void ACheckpoint::CheckEnabledState()
{
	for (auto Pair : EnemyVolumesState)
	{
		if (Pair.Value)
		{
			// Volume still has enemies spawned, disable collision and Checkpoint
			Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			bIsCheckpointEnabled = false;
			MinimapTrackerComponent->SetSprite(UnactiveSprite);
			UpdateEnabledState(bIsCheckpointEnabled);
			return;
		}
	}
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	bIsCheckpointEnabled = true;
	MinimapTrackerComponent->SetSprite(ActiveSprite);
	UpdateEnabledState(bIsCheckpointEnabled);

}

void ACheckpoint::TrackingVolumeIsClear(AAuraEnemySpawnVolume* SpawnVolume, bool bNewVolumeState)
{
	EnemyVolumesState.Add(SpawnVolume, bNewVolumeState);
	CheckEnabledState();
}

void ACheckpoint::RegisterSpawnVolumes()
{
	for (AAuraEnemySpawnVolume* SpawnVolume : EnemyVolumesToClearBeforeEnabling)
	{
		if (IsValid(SpawnVolume))
		{
			EnemyVolumesState.Add(SpawnVolume,  SpawnVolume->HasEnemiesAlive());
			if (SpawnVolume->HasEnemiesAlive())
				SpawnVolume->GetOnSpawnVolumeStateChangedDelegate().AddDynamic(this, &ACheckpoint::TrackingVolumeIsClear);
		}
	}
	bHasRegisteredSpawnVolumes = true;
}

void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();

	if (bBindOverlapCallback)
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnSphereOverlap);

	if (!bHasRegisteredSpawnVolumes)
	{
		RegisterSpawnVolumes();
	}
	CheckEnabledState();
}

void ACheckpoint::HandleGlowEffects()
{
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(CheckpointMesh->GetMaterial(0), this);

	CheckpointMesh->SetMaterial(0, DynamicMaterialInstance);

	CheckpointReached(DynamicMaterialInstance);
}
