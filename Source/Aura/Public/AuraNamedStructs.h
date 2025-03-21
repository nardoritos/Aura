#pragma once

/*
 * AuraNamedArguments
 *
 * Template for the named arguments used in the project
 */

struct FAuraNamedArguments
{
	FString _Name = FString("_Name"); //Ability Name/Title
	
	FString _Level0 = FString("_Level0"); // Current Level
	FString _Level1 = FString("_Level1"); // Next Level

	FString _Mana0 = FString("_Mana0"); // Current Mana Cost
	FString _Mana1 = FString("_Mana1"); // Next Mana Cost

	FString _CD0 = FString("_CD0"); // Current Cooldown
	FString _CD1 = FString("_CD1"); // Next Cooldown

	FString _FDmg0 = FString("_FDmg0"); // Current Damage
	FString _FDmg1 = FString("_FDmg1"); // Next Damage
	FString _FDmg0Half = FString("_FDmg0Half"); // Current Damage / 2
	FString _FDmg1Half = FString("_FDmg1Half"); // Next Damage / 2
	
	FString _NumProjectiles0 = FString("_NumProjectiles0"); // Quantity of Projectiles 
	FString _NumProjectiles1 = FString("_NumProjectiles1"); // Next Quantity of Projectiles
	
	FString _NumPropagatedTargets0 = FString("_NumPropagatedTargets0"); // Quantity of Propagated Targets 
	FString _NumPropagatedTargets1 = FString("_NumPropagatedTargets1"); // Next Quantity of Propagated Targets
	
	FString _PropagatedDamage0 = FString("_PropagatedDamage0"); // Current Propagating Damage
	FString _PropagatedDamage1 = FString("_PropagatedDamage1"); // Next Propagating Damage

	FString _RegenAmount0 = FString("_RegenAmount0"); // Current Regen Amount
	FString _RegenAmount1 = FString("_RegenAmount1"); // Next Regen Amount
	
	FString _Duration0 = FString("_Duration0"); // Current Ability Duration Amount
	FString _Duration1 = FString("_Duration1"); // Next Ability Duration Amount
	
	
};