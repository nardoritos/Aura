// Copyright (c) 2024, Nardoritos. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AuraUserWidget.h"
#include "OverlayWidget.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UOverlayWidget : public UAuraUserWidget
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdatePauseMenuState(bool bShouldOpenMenu);

	UFUNCTION(BlueprintCallable)
	void HandlePauseMenu();

	UPROPERTY(BlueprintReadOnly)
	bool bPauseMenuOpen = false;
};
