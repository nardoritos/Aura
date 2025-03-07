// Copyright (c) 2024, Nardoritos. All rights reserved.


#include "UI/HUD/LoadScreenHUD.h"

#include "Blueprint/UserWidget.h"
#include "UI/ViewModel/MVVM_LoadScreen.h"
#include "UI/Widget/LoadScreenWidget.h"

void ALoadScreenHUD::BeginPlay()
{
	Super::BeginPlay();	


}

void ALoadScreenHUD::CreateLoadScreenWidget()
{
	LoadScreenViewModel = NewObject<UMVVM_LoadScreen>(this, LoadScreenViewModelClass);
	LoadScreenViewModel->InitializeLoadSlots();
	
	LoadScreenWidget = CreateWidget<ULoadScreenWidget>(GetWorld(), LoadScreenWidgetClass);
	LoadScreenWidget->AddToViewport();

	LoadScreenWidget->BlueprintInitializeWidget();

	LoadScreenViewModel->LoadData();
}

void ALoadScreenHUD::DestroyLoadScreenWidget()
{
	if (LoadScreenWidget)
	{
		LoadScreenWidget->RemoveFromParent();
		LoadScreenWidget = nullptr;
	}
}
