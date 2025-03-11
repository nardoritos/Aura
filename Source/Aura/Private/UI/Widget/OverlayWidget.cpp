// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "UI/Widget/OverlayWidget.h"

void UOverlayWidget::HandlePauseMenu()
{
	bPauseMenuOpen = !bPauseMenuOpen;
	UpdatePauseMenuState(bPauseMenuOpen);
}
