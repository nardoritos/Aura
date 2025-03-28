// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "UI/Widget/AuraUserWidget.h"

#include "Fonts/FontMeasure.h"

void UAuraUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}

float UAuraUserWidget::GetTextSize(const FText Text, const FSlateFontInfo& TextFont) const
{
	const TSharedRef<FSlateFontMeasure>& FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

	return FontMeasure->Measure(Text, TextFont).X;
}
