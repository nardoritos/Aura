// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AuraNamedStructs.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"

bool UAuraAbilitySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject,
	FWidgetControllerParams& OutWCParams, AAuraHUD*& OutAuraHUD)
{
	if(APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		OutAuraHUD = Cast<AAuraHUD>(PC->GetHUD());
		if(OutAuraHUD)
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();

			OutWCParams.AttributeSet = AS;//(PC, PS, ASC, AS);
			OutWCParams.AbilitySystemComponent = ASC;
			OutWCParams.PlayerController = PC;
			OutWCParams.PlayerState = PS;
			return true;
		}
	}
	return false;
}



UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AAuraHUD* AuraHUD = nullptr;
	
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, AuraHUD))
		return AuraHUD->GetOverlayWidgetController(WCParams);
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AAuraHUD* AuraHUD = nullptr;
	
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, AuraHUD))
		return AuraHUD->GetAttributeMenuWidgetController(WCParams);
	return nullptr;
}

USpellMenuWidgetController* UAuraAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AAuraHUD* AuraHUD = nullptr;
	
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, AuraHUD))
		return AuraHUD->GetSpellMenuWidgetController(WCParams);
	return nullptr;
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject,
                                                            ECharacterClass CharacterClass, float Level,
                                                            UAbilitySystemComponent* ASC)
{
	AActor* AvatarActor = ASC->GetAvatarActor();
	
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	const FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, PrimaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, Level, SecondaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, Level, VitalAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
	
}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return;
	
	for(TSubclassOf<UGameplayAbility> AbilityClass:CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}
	
	const FCharacterClassDefaultInfo& DefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultInfo.StartupAbilities)
	{
		if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
		{
			int PlayerLevel = ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor());
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, PlayerLevel);
			ASC->GiveAbility(AbilitySpec);
		}
	}
}

int32 UAuraAbilitySystemLibrary::GetXPRewardForCharacterClassAndLevel(const UObject* WorldContextObject,
	ECharacterClass CharacterClass, int32 CharacterLevel)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return 0;

	const FCharacterClassDefaultInfo& Info = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	const float XPReward = Info.XPReward.GetValueAtLevel(CharacterLevel);

	return static_cast<int32>(XPReward);
}

void UAuraAbilitySystemLibrary::GetAbilityDescription(const UObject* WorldContextObject, UAuraAbilitySystemComponent* ASC,
		const FGameplayTag& AbilityTag, const int32 Level, FString& OutDescription, FString& OutNextLevelDescription)
{
	const FAuraAbilityInfo Info = GetAbilityInfo(WorldContextObject)->FindAbilityInfoForTag(AbilityTag);
	
	FText Description = FText::FromString(Info.GetDescription(Level));
	FText NextLevelDescription = FText::FromString(Info.NextDescription);
	FText LockedDescription = FText::FromString(Info.LockedDescription);
	
	UGameplayAbility* Ability = Info.Ability.GetDefaultObject();

	FText CommonDescription = FText::FromString(""
		"<Default>Cooldown: </><Cooldown>{_CD0}s</>\n"
		"<Default>Cost: </><ManaCost>{_Mana0}</>");
	
	FText CommonNextLevelDescription = FText::FromString(""
		"<Default>Cooldown: </><Old>{_CD0}s</>-><Cooldown>{_CD1}s</>\n"
		"<Default>Cost: </><Old>{_Mana0}</>-><ManaCost>{_Mana1}</>"); 
	
	
	if (const FGameplayAbilitySpec* AbilitySpec = ASC->GetSpecFromAbilityTag(AbilityTag))
	{
		if(Cast<UAuraGameplayAbility>(AbilitySpec->Ability))
		{
			FormatAbilityDescriptionAtLevel(Ability, Info, Level, Description);
			FormatAbilityDescriptionAtLevel(Ability, Info, Level, NextLevelDescription);
			FormatAbilityDescriptionAtLevel(Ability, Info, Level, CommonDescription);
			FormatAbilityDescriptionAtLevel(Ability, Info, Level, CommonNextLevelDescription);
			
			// Ability is Eligible, Unlocked or Equipped
			OutDescription = FString::Printf(TEXT(
				"<Title>%s</>\n"
				"<SubTitle>Level </><SubTitle>%i</>\n\n"
				"%s\n\n"
				"%s"
				
				),
				*Info.Name,
				Level,
				*Description.ToString(),
				*CommonDescription.ToString()
				);
			OutNextLevelDescription = FString::Printf(TEXT(
				"<Title>NEXT LEVEL:</>\n"
				"<SubTitle>Level </><Old>%i</><SubTitle>-></><Level>%i</>\n\n"
				"%s\n\n"
				"%s"
				
				),
				Level,
				Level + 1,
				*NextLevelDescription.ToString(),
				*CommonNextLevelDescription.ToString()
				);
			return;
		}
	}

	if (!AbilityTag.IsValid() || AbilityTag.MatchesTagExact(FAuraGameplayTags::Get().Abilities_None))
	{
		OutDescription = FString();
	}
	else
	{
		FormatAbilityDescriptionAtLevel(Ability, Info, Info.LevelRequirement, LockedDescription);
		// Ability is Locked
		OutDescription = FString::Printf(
			TEXT(
				"<Locked>%s</>\n"
			),
			*LockedDescription.ToString()
		);
	}	
	OutNextLevelDescription = FString();
		
}

void UAuraAbilitySystemLibrary::FormatAbilityDescriptionAtLevel(UGameplayAbility* Ability, const FAuraAbilityInfo& AbilityInfo, const int32 Level, FText& OutDescription)
{
	const FAuraNamedArguments Args;
	
	if (UAuraDamageGameplayAbility* DamageAbility = Cast<UAuraDamageGameplayAbility>(Ability))
	{
		OutDescription = FText::FormatNamed(
			OutDescription,
			Args._Level0, Level,
			Args._Level1, Level + 1,
			Args._FDmg0, DamageAbility->GetRoundedDamageAtLevel(Level, AbilityInfo.DamageTypeTag),
			Args._FDmg1, DamageAbility->GetRoundedDamageAtLevel(Level + 1, AbilityInfo.DamageTypeTag),
			Args._CD0, DamageAbility->GetCooldownAtLevel(Level),
			Args._CD1, DamageAbility->GetCooldownAtLevel(Level + 1),
			Args._Mana0, -DamageAbility->GetManaCostAtLevel(Level),
			Args._Mana1, -DamageAbility->GetManaCostAtLevel(Level + 1),
			Args._NumProjectiles0, AbilityInfo.GetSpecialAttribute(Level, NumProjectiles),
			Args._NumProjectiles1, AbilityInfo.GetSpecialAttribute(Level + 1, NumProjectiles)
		);
	}
	else
	{
		OutDescription = FText::FormatNamed(
			OutDescription,
			Args._Level0, Level,
			Args._Level1, Level + 1					
		);
	}
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if(AuraGameMode == nullptr) return nullptr;
	return AuraGameMode->CharacterClassInfo;
}

UAbilityInfo* UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if(AuraGameMode == nullptr) return nullptr;
	return AuraGameMode->AbilityInfo;
}

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsBlockedHit();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsCriticalHit();
	}
	return false;
}

void UAuraAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit)
{
	if(FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsBlockedHit(bInIsBlockedHit);
	}
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInIsCriticalHit)
{
	if(FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}

void UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject,
	TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius,
	const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);

	
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode()))
	{
		TArray<FOverlapResult> Overlaps;
		World->OverlapMultiByObjectType(Overlaps,
			SphereOrigin,
			FQuat::Identity,
			FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects),
			FCollisionShape::MakeSphere(Radius),
			SphereParams);
		for (FOverlapResult& Overlap : Overlaps)
		{

			if (Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Overlap.GetActor()))
			{
				OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatar(Overlap.GetActor()));
			}

			
		}
	}
	
}

bool UAuraAbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{

	// Redundancy is good sometimes
	const bool bFirstIsPlayer = FirstActor->ActorHasTag(FName("Player"));
	const bool bSecondIsPlayer = SecondActor->ActorHasTag(FName("Player"));
	const bool bFirstIsEnemy = FirstActor->ActorHasTag(FName("Enemy"));
	const bool bSecondIsEnemy = SecondActor->ActorHasTag(FName("Enemy"));

	const bool bBothArePlayers = bFirstIsPlayer && bSecondIsPlayer;
	const bool bBothAreEnemies = bFirstIsEnemy && bSecondIsEnemy;

	const bool bFriends = bBothArePlayers || bBothAreEnemies;

	return !bFriends;
	
}

