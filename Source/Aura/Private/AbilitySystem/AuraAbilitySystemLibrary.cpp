// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AuraNamedStructs.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/Abilities/AuraRegenerativePassiveAbility.h"
#include "Aura/AuraLogChannels.h"
#include "Engine/DamageEvents.h"
#include "Game/AuraGameModeBase.h"
#include "Game/AuraSaveGame.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"


class AAuraGameModeBase;
class UAuraDamageGameplayAbility;

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

void UAuraAbilitySystemLibrary::InitializeDefaultAttributesFromSaveData(const UObject* WorldContextObject,
	UAbilitySystemComponent* ASC, UAuraSaveGame* SaveGame)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return;

	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	const AActor* SourceAvatarActor = ASC->GetAvatarActor();

	FGameplayEffectContextHandle PrimaryAttributesFromSaveHandle = ASC->MakeEffectContext();
	PrimaryAttributesFromSaveHandle.AddSourceObject(SourceAvatarActor);

	const FGameplayEffectSpecHandle PrimaryAttributesFromSaveSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->PrimaryAttributes_SetByCaller, 1.f, PrimaryAttributesFromSaveHandle);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(PrimaryAttributesFromSaveSpecHandle, GameplayTags.Attributes_Primary_Strength, SaveGame->Strength);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(PrimaryAttributesFromSaveSpecHandle, GameplayTags.Attributes_Primary_Intelligence, SaveGame->Intelligence);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(PrimaryAttributesFromSaveSpecHandle, GameplayTags.Attributes_Primary_Vigor, SaveGame->Vigor);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(PrimaryAttributesFromSaveSpecHandle, GameplayTags.Attributes_Primary_Resilience, SaveGame->Resilience);

	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesFromSaveSpecHandle.Data.Get());
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
#define LOCTEXT_NAMESPACE "Abilities"
void UAuraAbilitySystemLibrary::GetAbilityDescription(UDataTable* AbilityInfo, UAuraAbilitySystemComponent* ASC,
                                                      const FGameplayTag& AbilityTag, const int32 Level, FString& OutDescription, FString& OutNextLevelDescription)
{
	const FAuraAbilityInfo Info = *GetDataTableRowByTag<FAuraAbilityInfo>(AbilityInfo, AbilityTag); 
	
	FText Description = Info.GetDescription(Level);
	FText NextLevelDescription = Info.NextDescription;
	FText LockedDescription = Info.LockedDescription;
	
	UGameplayAbility* Ability = Info.Ability.GetDefaultObject();
	
	FText CommonDescription = FText(LOCTEXT("AbilityCommonDescription",
		"<Default>Cooldown: </><Cooldown>{_CD0}s</>\n"
		"<Default>Cost: </><ManaCost>{_Mana0}</>"));
	
	FText CommonNextLevelDescription = FText(LOCTEXT("AbilityCommonNextLevelDescription",
		"<Default>Cooldown: </><Old>{_CD0}s</>-><Cooldown>{_CD1}s</>\n"
		"<Default>Cost: </><Old>{_Mana0}</>-><ManaCost>{_Mana1}</>")); 
	const FText LevelText = FText(LOCTEXT("AbilityLevelText", "Level"));
	const FText NextLevelText = FText(LOCTEXT("AbilityNextLevelText", "NEXT LEVEL"));
	
	if (const FGameplayAbilitySpec* AbilitySpec = ASC->GetSpecFromAbilityTag(AbilityTag))
	{
		if(Cast<UAuraDamageGameplayAbility>(AbilitySpec->Ability))
		{
			FormatAbilityDescriptionAtLevel(Ability, Info, Level, Description);
			FormatAbilityDescriptionAtLevel(Ability, Info, Level, NextLevelDescription);
			FormatAbilityDescriptionAtLevel(Ability, Info, Level, CommonDescription);
			FormatAbilityDescriptionAtLevel(Ability, Info, Level, CommonNextLevelDescription);
			
			// Ability is Eligible, Unlocked or Equipped
			OutDescription = FString::Printf(TEXT(
				"<Title>%s</>\n"
				"<SubTitle>%s </><Level>%i</>\n\n"
				"%s\n\n"
				"%s"
				
				),
				*Info.Name.ToString(),
				*LevelText.ToString(),
				Level,
				*Description.ToString(),
				*CommonDescription.ToString()
				);
			OutNextLevelDescription = FString::Printf(TEXT(
				"<Title>%s:</>\n"
				"<SubTitle>Level </><Old>%i</><SubTitle>-></><Level>%i</>\n\n"
				"%s\n\n"
				"%s"
				
				),
				*NextLevelText.ToString(),
				Level,
				Level + 1,
				*NextLevelDescription.ToString(),
				*CommonNextLevelDescription.ToString()
				);
			return;
		}
		if (Cast<UAuraPassiveAbility>(AbilitySpec->Ability))
		{
			FormatAbilityDescriptionAtLevel(Ability, Info, Level, Description);
			FormatAbilityDescriptionAtLevel(Ability, Info, Level, NextLevelDescription);
			
			// Ability is Eligible, Unlocked or Equipped
			OutDescription = FString::Printf(TEXT(
				"<Title>%s</>\n"
				"<SubTitle>Level </><Level>%i</>\n\n"
				"%s"
				
				),
				*Info.Name.ToString(),
				Level,
				*Description.ToString()
				);
			OutNextLevelDescription = FString::Printf(TEXT(
				"<Title>NEXT LEVEL:</>\n"
				"<SubTitle>Level </><Old>%i</><SubTitle>-></><Level>%i</>\n\n"
				"%s"
				
				),
				Level,
				Level + 1,
				*NextLevelDescription.ToString()
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
#undef LOCTEXT_NAMESPACE
void UAuraAbilitySystemLibrary::SetIsRadialDamageEffectParam(FDamageEffectParams& DamageEffectParams, bool bIsRadial, float InnerRadius, float OuterRadius, FVector Origin)
{
	DamageEffectParams.bIsRadialDamage = bIsRadial;
	DamageEffectParams.RadialDamageInnerRadius = InnerRadius;
	DamageEffectParams.RadialDamageOuterRadius = OuterRadius;
	DamageEffectParams.RadialDamageOrigin = Origin;
}

void UAuraAbilitySystemLibrary::SetKnockbackDirection(FDamageEffectParams& DamageEffectParams, FVector KnockbackDirection, float Magnitude)
{
	KnockbackDirection.Normalize();
	if (Magnitude == 0.f)
	{
		DamageEffectParams.KnockbackForce = KnockbackDirection * DamageEffectParams.KnockbackForceMagnitude;
	}
	else
	{
		DamageEffectParams.KnockbackForce = KnockbackDirection * Magnitude;
	}
}

void UAuraAbilitySystemLibrary::SetDeathImpulseDirection(FDamageEffectParams& DamageEffectParams, FVector ImpulseDirection, float Magnitude)
{
	ImpulseDirection.Normalize();
	if (Magnitude == 0.f)
	{
		DamageEffectParams.DeathImpulse = ImpulseDirection * DamageEffectParams.DeathImpulseMagnitude;
	}
	else
	{
		DamageEffectParams.DeathImpulse = ImpulseDirection * Magnitude;
	}
}

void UAuraAbilitySystemLibrary::SetTargetEffectParamsASC(FDamageEffectParams& DamageEffectParams,
	UAbilitySystemComponent* InASC)
{
	DamageEffectParams.TargetAbilitySystemComponent = InASC;
}

void UAuraAbilitySystemLibrary::FormatAbilityDescriptionAtLevel(UGameplayAbility* Ability, const FAuraAbilityInfo& AbilityInfo, const int32 Level, FText& OutDescription)
{
	const FAuraNamedArguments Args;	
	
	if (const UAuraDamageGameplayAbility* DamageAbility = Cast<UAuraDamageGameplayAbility>(Ability))
	{
		OutDescription = FText::FormatNamed(
			OutDescription,
			Args._Level0, Level,
			Args._Level1, Level + 1,
			Args._FDmg0, SanitizeFloat(DamageAbility->GetDamageAtLevel(Level)),
			Args._FDmg1, SanitizeFloat(DamageAbility->GetDamageAtLevel(Level + 1)),
			Args._FDmg0Half, SanitizeFloat(DamageAbility->GetDamageAtLevel(Level) / 2),
			Args._FDmg1Half, SanitizeFloat(DamageAbility->GetDamageAtLevel(Level + 1) / 2),
			Args._CD0, SanitizeFloat(DamageAbility->GetCooldownAtLevel(Level)),
			Args._CD1, SanitizeFloat(DamageAbility->GetCooldownAtLevel(Level + 1)),
			Args._Mana0, SanitizeFloat(-DamageAbility->GetManaCostAtLevel(Level)),
			Args._Mana1, SanitizeFloat(-DamageAbility->GetManaCostAtLevel(Level + 1)),
			Args._PropagatedDamage0, SanitizeFloat(DamageAbility->GetPropagatedDamageAtLevel(Level)),
            Args._PropagatedDamage1, SanitizeFloat(DamageAbility->GetPropagatedDamageAtLevel(Level + 1)),
			Args._NumProjectiles0, AbilityInfo.GetSpecialAttribute(Level, NumProjectiles),
			Args._NumProjectiles1, AbilityInfo.GetSpecialAttribute(Level + 1, NumProjectiles),
			Args._NumPropagatedTargets0, AbilityInfo.GetSpecialAttribute(Level, NumPropagatedTargets),
			Args._NumPropagatedTargets1, AbilityInfo.GetSpecialAttribute(Level + 1, NumPropagatedTargets),
			Args._Duration0, SanitizeFloat(AbilityInfo.GetSpecialAttribute(Level, Duration)),
			Args._Duration1, SanitizeFloat(AbilityInfo.GetSpecialAttribute(Level + 1, Duration)),
			Args._Radius0, SanitizeFloat(AbilityInfo.GetSpecialAttribute(Level, Radius)),
			Args._Radius1, SanitizeFloat(AbilityInfo.GetSpecialAttribute(Level + 1, Radius))
		);
	}
	else if (const UAuraRegenerativePassiveAbility* RegenAbility = Cast<UAuraRegenerativePassiveAbility>(Ability))
	{
		OutDescription = FText::FormatNamed(
			OutDescription,
			Args._Level0, Level,
			Args._Level1, Level + 1,
			Args._RegenAmount0, SanitizeFloat(RegenAbility->RegenAmount.GetValueAtLevel(Level)),
			Args._RegenAmount1, SanitizeFloat(RegenAbility->RegenAmount.GetValueAtLevel(Level + 1))
		);
	}
	else
	{
		OutDescription = FText::FormatNamed(
			OutDescription,
			Args._Level0, Level,
			Args._Level1, Level + 1,
			Args._Effect0, SanitizeFloat(AbilityInfo.GetSpecialAttribute(Level, EffectMagnitude)),
			Args._Effect1, SanitizeFloat(AbilityInfo.GetSpecialAttribute(Level + 1, EffectMagnitude))
		);
	}
}

FText UAuraAbilitySystemLibrary::SanitizeFloat(const float OriginalValue)
{
	return FText::FromString(FString::Printf(TEXT("%.1f"), OriginalValue));
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if(AuraGameMode == nullptr) return nullptr;
	return AuraGameMode->CharacterClassInfo;
}

UDataTable* UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if(AuraGameMode == nullptr) return nullptr;
	return AuraGameMode->AbilityInfo;
}

ULootTiers* UAuraAbilitySystemLibrary::GetLootTiers(const UObject* WorldContextObject)
{
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if(AuraGameMode == nullptr) return nullptr;
	return AuraGameMode->LootTiers;
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

bool UAuraAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsSuccessfulDebuff();
	}
	return false;
}

float UAuraAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDebuffDamage();
	}
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDebuffDuration();
	}
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDebuffFrequency();
	}
	return 0.f;
}

FGameplayTag UAuraAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		if(AuraEffectContext->GetDamageType().IsValid())
		{
			return *AuraEffectContext->GetDamageType();
		}
	}
	return FGameplayTag();
}

FVector UAuraAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDeathImpulse();
	}
	return FVector::ZeroVector;
}

bool UAuraAbilitySystemLibrary::ShouldHitReact(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->ShouldHitReact();
	}
	return true;
}

FVector UAuraAbilitySystemLibrary::GetKnockbackForce(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetKnockbackForce();
	}
	return FVector::ZeroVector;
}

bool UAuraAbilitySystemLibrary::IsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsRadialDamage();
	}
	return false;
}

float UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetRadialDamageInnerRadius();
	}
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetRadialDamageOuterRadius();
	}
	return 0.f;
}

FVector UAuraAbilitySystemLibrary::GetRadialDamageOrigin(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetRadialDamageOrigin();
	}
	return FVector::ZeroVector;
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

void UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInIsSuccessfulDebuff)
{
	if(FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsSuccessfulDebuff(bInIsSuccessfulDebuff);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle, float InDebuffDamage)
{
	if(FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDebuffDamage(InDebuffDamage);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDuration(FGameplayEffectContextHandle& EffectContextHandle,
	float InDebuffDuration)
{
	if(FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDebuffDuration(InDebuffDuration);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& EffectContextHandle,
	float InDebuffFrequency)
{
	if(FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDebuffFrequency(InDebuffFrequency);
	}
}

void UAuraAbilitySystemLibrary::SetDamageType(FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& InDamageType)
{
	if(FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		const TSharedPtr<FGameplayTag> DamageType = MakeShared<FGameplayTag>(InDamageType);
		AuraEffectContext->SetDamageType(DamageType);
	}
}

void UAuraAbilitySystemLibrary::SetDeathImpulse(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& InImpulse)
{
	if(FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDeathImpulse(InImpulse);
	}
}

void UAuraAbilitySystemLibrary::SetShouldHitReact(FGameplayEffectContextHandle& EffectContextHandle,
	const bool InShouldHitReact)
{
	if(FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetShouldHitReact(InShouldHitReact);
	}
}

void UAuraAbilitySystemLibrary::SetKnockbackForce(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& InKnockbackForce)
{
	if(FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetKnockbackForce(InKnockbackForce);
	}
}

void UAuraAbilitySystemLibrary::SetIsRadialDamage(FGameplayEffectContextHandle& EffectContextHandle,
	const bool InIsRadialDamage)
{
	if(FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsRadialDamage(InIsRadialDamage);
	}
}

void UAuraAbilitySystemLibrary::SetRadialDamageInnerRadius(FGameplayEffectContextHandle& EffectContextHandle,
	const float InRadialDamageInnerRadius)
{
	if(FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetRadialDamageInnerRadius(InRadialDamageInnerRadius);
	}
}

void UAuraAbilitySystemLibrary::SetRadialDamageOuterRadius(FGameplayEffectContextHandle& EffectContextHandle,
	const float InRadialDamageOuterRadius)
{
	if(FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetRadialDamageOuterRadius(InRadialDamageOuterRadius);
	}
}

void UAuraAbilitySystemLibrary::SetRadialDamageOrigin(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& InRadialDamageOrigin)
{
	if(FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetRadialDamageOrigin(InRadialDamageOrigin);
	}
}

void UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject,
                                                           TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius,
                                                           const FVector& SphereOrigin, const bool DrawDebug)
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
		if (DrawDebug)
		{
			UKismetSystemLibrary::DrawDebugSphere(
				WorldContextObject,
				SphereOrigin,
				Radius,
				12,
				FColor::White,
				5.f
				);
		}
		for (FOverlapResult& Overlap : Overlaps)
		{

			if (Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Overlap.GetActor()))
			{
				OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatar(Overlap.GetActor()));
			}

			
		}
	}
	
}

void UAuraAbilitySystemLibrary::GetClosestTargets(int32 MaxTargets, const TArray<AActor*>& Actors,
	TArray<AActor*>& OutClosestTargets,	const FVector& Origin)
{
	if (Actors.Num() <= MaxTargets)
	{
		OutClosestTargets = Actors;
		return;
	}

	TArray<AActor*> ActorsToCheck = Actors;
	int32 NumTargetsFound = 0;

	while (NumTargetsFound < MaxTargets)
	{
		if (ActorsToCheck.Num() == 0) break;
		double ClosestDistance = TNumericLimits<double>::Max();
		AActor* ClosestActor;
		for (AActor* PotentialTarget : ActorsToCheck)
		{
			const double Distance = (PotentialTarget->GetActorLocation() - Origin).Length();
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestActor = PotentialTarget;
			}
		}
		ActorsToCheck.Remove(ClosestActor);
		OutClosestTargets.AddUnique(ClosestActor);
		++NumTargetsFound;
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

FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams)
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();	
	UAbilitySystemComponent* ASCToUse = DamageEffectParams.SourceAbilitySystemComponent ?
											DamageEffectParams.SourceAbilitySystemComponent :
											DamageEffectParams.TargetAbilitySystemComponent;


	// Changed from always using SourceAvatar to deciding to which one's available, so it can be used in other objects
	const AActor* SourceAvatarActor = ASCToUse->GetAvatarActor();
	FGameplayEffectContextHandle EffectContextHandle = ASCToUse->MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceAvatarActor);

	SetDeathImpulse(EffectContextHandle, DamageEffectParams.DeathImpulse);
	SetKnockbackForce(EffectContextHandle, DamageEffectParams.KnockbackForce);
	SetShouldHitReact(EffectContextHandle, DamageEffectParams.bShouldHitReact);
	SetIsRadialDamage(EffectContextHandle, DamageEffectParams.bIsRadialDamage);
	SetRadialDamageInnerRadius(EffectContextHandle, DamageEffectParams.RadialDamageInnerRadius);
	SetRadialDamageOuterRadius(EffectContextHandle, DamageEffectParams.RadialDamageOuterRadius);
	SetRadialDamageOrigin(EffectContextHandle, DamageEffectParams.RadialDamageOrigin);

	FGameplayEffectSpecHandle SpecHandle = ASCToUse->MakeOutgoingSpec(
		DamageEffectParams.DamageGameplayEffectClass, DamageEffectParams.AbilityLevel, EffectContextHandle);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageEffectParams.DamageType, DamageEffectParams.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Chance, DamageEffectParams.DebuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Damage, DamageEffectParams.DebuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Duration, DamageEffectParams.DebuffDuration);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Frequency, DamageEffectParams.DebuffFrequency);

	DamageEffectParams.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);

	return EffectContextHandle;	
}

FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyRegenEffect(
	const FRegenEffectParams& HealingEffectParams)
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();	
	UAbilitySystemComponent* ASCToHeal = HealingEffectParams.TargetAbilitySystemComponent;

	// Changed from always using SourceAvatar to deciding to which one's available, so it can be used in other objects
	const AActor* TargetAvatarActor = ASCToHeal->GetAvatarActor();
	FGameplayEffectContextHandle EffectContextHandle = ASCToHeal->MakeEffectContext();
	EffectContextHandle.AddSourceObject(TargetAvatarActor);
	
	FGameplayEffectSpecHandle SpecHandle = ASCToHeal->MakeOutgoingSpec(
		HealingEffectParams.RegenGameplayEffectClass, HealingEffectParams.AbilityLevel, EffectContextHandle);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,
		HealingEffectParams.RegenType, HealingEffectParams.BaseRegen.GetValueAtLevel(HealingEffectParams.AbilityLevel));
	
	HealingEffectParams.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);

	return EffectContextHandle;	
}

FActiveGameplayEffectHandle UAuraAbilitySystemLibrary::ApplyIndefiniteEffect(const FIndefiniteEffectParams& IndefiniteEffectParams, bool& AppliedSuccessfully)
{
	if (!IndefiniteEffectParams.TargetAbilitySystemComponent)
	{
		AppliedSuccessfully = false;
		return FActiveGameplayEffectHandle();
	}

	UAbilitySystemComponent* TargetToApply = IndefiniteEffectParams.TargetAbilitySystemComponent;
	const TSubclassOf<UGameplayEffect> EffectToApply = IndefiniteEffectParams.IndefiniteEffectClass;
	const float AbilityLevel = IndefiniteEffectParams.AbilityLevel;
	const FScalableFloat BaseMagnitude = IndefiniteEffectParams.BaseMagnitude;
	
	const AActor* TargetAvatarActor = TargetToApply->GetAvatarActor();
	FGameplayEffectContextHandle EffectContextHandle = TargetToApply->MakeEffectContext();
	EffectContextHandle.AddSourceObject(TargetAvatarActor);

	const FGameplayEffectSpecHandle SpecHandle = TargetToApply->MakeOutgoingSpec(
		EffectToApply, AbilityLevel, EffectContextHandle);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,
		IndefiniteEffectParams.Attribute, BaseMagnitude.GetValueAtLevel(AbilityLevel));

	AppliedSuccessfully = true;
	return TargetToApply->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
}


TArray<FRotator> UAuraAbilitySystemLibrary::EvenlySpacedRotators(const FVector& Forward, const FVector& Axis,
                                                                 const float Spread, const int32 NumRotators)
{
	TArray<FRotator> Rotators;
	
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis);
	if (NumRotators > 1)
	{
		const float DeltaSpread = Spread / (NumRotators - 1);
		// Multiple Rotators
		for (int32 i = 0; i < NumRotators; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, Axis);
			Rotators.Add(Direction.Rotation());
		}
	}
	else
	{
		// Single Rotator
		Rotators.Add(Forward.Rotation());
	}
	return Rotators;
}

TArray<FVector> UAuraAbilitySystemLibrary::EvenlyRotatedVectors(const FVector& Forward, const FVector& Axis,
	const float Spread, const int32 NumVectors)
{
	TArray<FVector> Vectors;
	
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis);
	if (NumVectors > 1)
	{
		const float DeltaSpread = Spread / (NumVectors - 1);
		// Multiple Vectors
		for (int32 i = 0; i < NumVectors; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, Axis);
			Vectors.Add(Direction);
		}
	}
	else
	{
		// Single Vectors
		Vectors.Add(Forward);
	}
	return Vectors;
}

FVector UAuraAbilitySystemLibrary::GetRandomPointInRadius(const FVector& Origin, const float Radius)
{
	const FVector2D RandomPoint2D = FMath::RandPointInCircle(Radius);
	const FVector RandomPoint3D = FVector(RandomPoint2D.X, RandomPoint2D.Y, 0); 
	return Origin + RandomPoint3D;
}

float UAuraAbilitySystemLibrary::GetValueAtLevel(const FScalableFloat& ScalableFloat, const float InLevel)
{
	return ScalableFloat.GetValueAtLevel(InLevel);
}
