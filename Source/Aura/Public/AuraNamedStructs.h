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

	FString _NumProjectiles0 = FString("_NumProjectiles0"); // Quantity of Projectiles 
	FString _NumProjectiles1 = FString("_NumProjectiles1"); // Next Quantity of Projectiles 
	
};