// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

class UPassiveNiagaraComponent;
class UDebuffNiagaraComponent;
class UNiagaraSystem;
class UGameplayAbility;
class UGameplayEffect;
class UAbilitySystemComponent;
class UAttributeSet;
class UAnimMontage;

UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
		
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	AAuraCharacterBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }
	
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath(const FVector& DeathImpulse);

	UPROPERTY(EditAnywhere, Category = "Aura|Combat")
	TArray<FTaggedMontage> AttackMontages;

	UPROPERTY(ReplicatedUsing=OnRep_Stunned, BlueprintReadOnly)
	bool bIsStunned = false;

	UPROPERTY(ReplicatedUsing=OnRep_Burned, BlueprintReadOnly)
	bool bIsBurned = false;
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsBeingShocked = false;

	UFUNCTION()
	virtual void OnRep_Stunned();

	UFUNCTION()
	virtual void OnRep_Burned();
	
	void SetCharacterClass(ECharacterClass InClass) {CharacterClass = InClass; }
	
protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|Combat")
	TObjectPtr<USkeletalMeshComponent> SkeletalWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|Combat")
	TObjectPtr<UStaticMeshComponent> StaticWeapon;
	
	UPROPERTY(EditAnywhere, Category = "Aura|Combat")
	FName WeaponTipSocketName;

	UPROPERTY(EditAnywhere, Category = "Aura|Combat")
	FName RightHandSocketName;

	UPROPERTY(EditAnywhere, Category = "Aura|Combat")
	FName LeftHandSocketName;

	UPROPERTY(EditAnywhere, Category = "Aura|Combat")
	FName OtherSocketName;

	UPROPERTY(BlueprintReadOnly, Category = "Aura|Combat")
	bool bDead = false;
	
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|Combat")
	float BaseWalkSpeed = 600.f;

public:
	//** CombatInterface */
	virtual UAnimMontage* GetHitReactMontage_Implementation() override;
	virtual void Die(const FVector& DeathImpulse, AActor* KillingActor) override;
	virtual FOnDeathSignature& GetOnDeathDelegate() override;
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;
	virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) override;
	virtual int32 GetMinionCount_Implementation() override;
	virtual void IncrementMinionCount_Implementation(int32 Amount) override;
	virtual void DecrementMinionCount_Implementation(int32 Amount) override;
	virtual ECharacterClass GetCharacterClass_Implementation() override;
	virtual FOnASCRegistered& GetOnASCRegisteredDelegate() override;
	virtual UMeshComponent* GetWeapon_Implementation() override;
	virtual bool IsBeingShocked_Implementation() const override;
	virtual void SetIsBeingShocked_Implementation(const bool bInIsBeingShocked) override;
	//** End CombatInterface */

protected:
	FOnASCRegistered OnASCRegistered;
	FOnDeathSignature OnDeathDelegate;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	virtual void InitAbilityActorInfo();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Aura|Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Aura|Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Aura|Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;
	
	void ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& GameplayEffectClass, float Level) const;
	virtual void InitializeDefaultAttributes() const;

	void AddCharacterAbilities();

	/* Dissolve Effects */

	void Dissolve();

	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|VFX")
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|VFX")
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|Combat")
	UNiagaraSystem* BloodEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|Combat")
	USoundBase* DeathSound;

	/* Minions */

	int32 MinionCount = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDebuffNiagaraComponent> BurnDebuffComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDebuffNiagaraComponent> StunDebuffComponent;
	
private:

	UPROPERTY(EditAnywhere, Category = "Aura|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditAnywhere, Category = "Aura|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupPassiveAbilities;

	// These effects are only applied one time on startup
	UPROPERTY(EditAnywhere, Category = "Aura|Abilities")
	TArray<TSubclassOf<UGameplayEffect>> StartupEffects;
	
	UPROPERTY(EditAnywhere, Category = "Aura|Combat")
	TObjectPtr<UAnimMontage> HitReactMontage; 

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> HaloOfProtectionNiagaraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> LifeSiphonNiagaraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> ManaSiphonNiagaraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> EffectAttachComponent;
	
};
