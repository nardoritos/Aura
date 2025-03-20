// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * AuraGameplayTags
 *
 * Singleton containing native GameplayTags
 */

struct FAuraGameplayTags
{
public:
    static const FAuraGameplayTags& Get() { return GameplayTags; }
    static void InitializeNativeGameplayTags();

	// Primary Attributes Tags
	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Resilience;
	FGameplayTag Attributes_Primary_Vigor;

	// Secondary Attributes Tags
	FGameplayTag Attributes_Secondary_Armor;
	FGameplayTag Attributes_Secondary_ArmorPenetration;
	FGameplayTag Attributes_Secondary_BlockChance;
	FGameplayTag Attributes_Secondary_CriticalHitChance;
	FGameplayTag Attributes_Secondary_CriticalHitDamage;
	FGameplayTag Attributes_Secondary_CriticalHitResistance;
	FGameplayTag Attributes_Secondary_HealthRegeneration;
	FGameplayTag Attributes_Secondary_ManaRegeneration;
	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_MaxMana;

	// Attributes Resistance Tags
	FGameplayTag Attributes_Resistance_Fire;
	FGameplayTag Attributes_Resistance_Lightning;
	FGameplayTag Attributes_Resistance_Arcane;
	FGameplayTag Attributes_Resistance_Physical;

	// Meta Attributes
	FGameplayTag Attributes_Meta_IncomingXP;
	

	// Input Tags
	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;
	FGameplayTag InputTag_Passive_1;
	FGameplayTag InputTag_Passive_2;

	// Damage Types
	FGameplayTag Damage;
	FGameplayTag Damage_Fire;
	FGameplayTag Damage_Lightning;
	FGameplayTag Damage_Arcane;
	FGameplayTag Damage_Physical;

	// Debuff Types
	FGameplayTag Debuff_Burn;
	FGameplayTag Debuff_Stun;
	FGameplayTag Debuff_Curse;
	FGameplayTag Debuff_Bleed;

	// Debuff Details
	FGameplayTag Debuff_Chance;
	FGameplayTag Debuff_Damage;
	FGameplayTag Debuff_Duration;
	FGameplayTag Debuff_Frequency;
	
	// Ability Types
	FGameplayTag Abilities;
	
	FGameplayTag Abilities_None;
	
	FGameplayTag Abilities_Attack;
	FGameplayTag Abilities_Summon;
	
	FGameplayTag Abilities_HitReact;

	FGameplayTag Abilities_Status_Locked;
	FGameplayTag Abilities_Status_Unlocked;
	FGameplayTag Abilities_Status_Eligible;
	FGameplayTag Abilities_Status_Equipped;

	FGameplayTag Abilities_Type_Offensive;
	FGameplayTag Abilities_Type_Passive;
	FGameplayTag Abilities_Type_None;

	FGameplayTag Abilities_WaitingExecution;

	// Ability Types->Fire
	FGameplayTag Abilities_Fire;
	FGameplayTag Abilities_Fire_FireBolt;
	FGameplayTag Abilities_Fire_FireBlast;
	FGameplayTag Abilities_Fire_FireNado;
	
	// Ability Types->Fire_Environment
	FGameplayTag Abilities_Fire_DelayedExplosion;
	

	// Ability Types->Lightning
	FGameplayTag Abilities_Lightning;
	FGameplayTag Abilities_Lightning_Electrocute;

	// Ability Types->Arcane
	FGameplayTag Abilities_Arcane;
	FGameplayTag Abilities_Arcane_ArcaneShards;
	
	// Ability Cooldowns->Fire
	FGameplayTag Cooldown_Fire_FireBolt;
	FGameplayTag Cooldown_Fire_FireBlast;
	
	// Ability Cooldowns->Lightning
	FGameplayTag Cooldown_Lightning_Electrocute;

	// Ability Cooldowns->Arcane
	FGameplayTag Cooldown_Arcane_ArcaneShards;
	
	// Passive Abilities
	FGameplayTag Abilities_Passive_HaloOfProtection;
	FGameplayTag Abilities_Passive_LifeSiphon;
	FGameplayTag Abilities_Passive_ManaSiphon;
	
	// Combat Sockets Tags
	FGameplayTag CombatSocket_Weapon;
	FGameplayTag CombatSocket_RightHand;
	FGameplayTag CombatSocket_LeftHand;
	FGameplayTag CombatSocket_Other;

	// Montages Tags
	FGameplayTag Montage_Attack_1;
	FGameplayTag Montage_Attack_2;
	FGameplayTag Montage_Attack_3;
	FGameplayTag Montage_Attack_4;
	
	
	// Maps each Damage Type Tag with their own Resistance Tag
	TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances;

	// Maps each Damage Type Tag with their own Debuff Tag 
	TMap<FGameplayTag, FGameplayTag> DamageTypesToDebuffs;

	// Hit React Tag
	FGameplayTag Effects_HitReact;

	// Player Blocking Tags
	FGameplayTag Player_Block_InputPressed;
	FGameplayTag Player_Block_InputHeld;
	FGameplayTag Player_Block_InputReleased;
	FGameplayTag Player_Block_CursorTrace;

	// Gameplay Cue
	FGameplayTag GameplayCue_FireBlast;
	
private:
    static FAuraGameplayTags GameplayTags;
};