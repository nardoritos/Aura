// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once
#include "Interaction/CombatInterface.h"
#include "Interaction/HighlightInterface.h"
#include "Interaction/SaveInterface.h"

#include "DamageableActor.generated.h"

class UBoxComponent;
class UGameplayEffect;
class UAbilitySystemComponent;
class UAttributeSet;

UCLASS()
class AURA_API ADamageableActor : public AActor, public ICombatInterface, public IHighlightInterface, public ISaveInterface
{
	GENERATED_BODY()
	
public:	
	
	ADamageableActor();

protected:

	/* Combat Interface */
	virtual void Die(const FVector& DeathImpulse) override;
	virtual FOnASCRegistered& GetOnASCRegisteredDelegate() override;
	virtual FOnDeathSignature& GetOnDeathDelegate() override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	/* End Combat Interface */

	/* Save Interface */
	virtual void LoadActor_Implementation() override;
	/* End Save Interface */
	
	/* Highlight Interface */
	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;
	/* End Highlight Interface */
	
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnLoot();

	UPROPERTY(SaveGame)
	bool bDead = false;
	

	
	UAbilitySystemComponent* GetAbilitySystemComponent() const;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }
	
	virtual void BeginPlay() override;
	void InitAbilityActorInfo();

	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_HandleDeath();

	FOnDeathSignature OnDeathDelegate;
	FOnASCRegistered OnASCRegistered;
	
	void ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& GameplayEffectClass, float Level) const;
	virtual void InitializeDefaultAttributes() const;

	void RegisterChildrenMeshes();

	void OnHealthChanged(const FOnAttributeChangeData& Data) const;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	
	/* Dissolve Effects */

	void Dissolve();
 
	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(const TArray<UMaterialInstanceDynamic*>& DynamicMaterialInstances);
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Aura|Abilities")
	TSubclassOf<UGameplayEffect> DefaultDamageableAttributes;
	
	UPROPERTY(VisibleAnywhere, Category = "Aura|Abilities")
	TMap<TSubclassOf<UGameplayAbility>, FGameplayTag> OnDeathAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Aura|Abilities")
	bool bActivatesOnProximity = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura")
	float LifeSpan = 5.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY()
	TArray<UStaticMeshComponent*> Meshes;

	UPROPERTY(EditDefaultsOnly, Category = "Aura|Audio")
	TObjectPtr<USoundBase> DeathSound;

	UPROPERTY(EditDefaultsOnly, Category = "Aura|Audio")
	TObjectPtr<USoundBase> HurtSound;

	UPROPERTY(EditDefaultsOnly, Category = "Aura|Audio")
	TObjectPtr<USoundBase> BlockSound;


};

