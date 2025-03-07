// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "UI/Widget/LoadScreenWidget.h"

#include "UI/HUD/LoadScreenHUD.h"


ALoadScreenHUD* ULoadScreenWidget::GetLoadScreenHUD() const
{
	check(GetOwningPlayer())

	check(GetOwningPlayer()->GetHUD())
	
	return Cast<ALoadScreenHUD>(GetOwningPlayer()->GetHUD());
}
