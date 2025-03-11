// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UNiagaraComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()

public:
	AAuraCharacter();
	
	virtual void PossessedBy(AController* NewController) override;
	void LoadProgress();
	virtual void OnRep_PlayerState() override;
	virtual void OnRep_Stunned() override;
	virtual void OnRep_Burned() override;
	
	/** Player Interface */
	virtual int32 FindLevelForXP_Implementation(int32 InXP) const override;
	virtual int32 GetXP_Implementation() override;
	virtual void AddToXP_Implementation(int32 InXP) override;
	virtual void LevelUp_Implementation() override;
	virtual int32 GetAttributePointsReward_Implementation(int32 Level) const override;
	virtual int32 GetSpellPointsReward_Implementation(int32 Level) const override;
	virtual void AddToPlayerLevel_Implementation(int32 InPlayerLevel) override;
	virtual void AddToAttributePoints_Implementation(int32 InAttributePoints) override;
	virtual int32 GetAttributePoints_Implementation() const override;
	virtual void AddToSpellPoints_Implementation(int32 InSpellPoints) override;
	virtual int32 GetSpellPoints_Implementation() const override;
	virtual void ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial = nullptr) override;
	virtual void HideMagicCircle_Implementation() override;
	virtual void SaveProgress_Implementation(const FName& CheckpointTag) override;
	/** End Player Interface */
	
	/** Combat Interface */
	virtual	int32 GetPlayerLevel_Implementation() override;
	virtual void Die(const FVector& DeathImpulse) override;
	/** End Combat Interface*/

	UPROPERTY(EditDefaultsOnly)
	float DeathTime = 5.f;

	FTimerHandle DeathTimer;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> TopDownCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USpringArmComponent> CameraBoom;
	
private:

	virtual void InitAbilityActorInfo() override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLevelUpParticles() const;
};
