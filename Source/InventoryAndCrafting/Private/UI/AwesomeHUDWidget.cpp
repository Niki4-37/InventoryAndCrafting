// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/AwesomeHUDWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Components/CanvasPanel.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/InventoryComponent.h"
#include "UI/AwesomeCraftingDeckWidget.h"
#include "UI/AwesomeDropBoxWidget.h"
#include "UI/AwesomeEquipmentWidget.h"
#include "UI/InventoryWidget.h"
#include "UI/PersonalSlotsWidget.h"
#include "UI/ShopWidget.h"

void UAwesomeHUDWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (SwitchToCraftingButton)
    {
        SwitchToCraftingButton->OnClicked.AddDynamic(this, &UAwesomeHUDWidget::OnSwitchCraftingPanel);
    }
    if (BackButton)
    {
        BackButton->OnClicked.AddDynamic(this, &UAwesomeHUDWidget::OnGoBack);
    }
    if (WidgetSwitcherBetween && InventoryAndEquipmentPanel)
    {
        WidgetSwitcherBetween->SetActiveWidget(InventoryAndEquipmentPanel);
    }
    if (GetOwningPlayer())
    {
        GetOwningPlayer()->GetOnNewPawnNotifier().AddUObject(this, &UAwesomeHUDWidget::OnNewPawn);
        OnNewPawn(GetOwningPlayerPawn());
    }

    InitWidget();
}

void UAwesomeHUDWidget::OnNewPawn(APawn* NewPawn)
{
    if (!NewPawn) return;
    const auto InventoryComponent = NewPawn->FindComponentByClass<UInventoryComponent>();
    if (!InventoryComponent) return;
    if (!InventoryComponent->OnTrading.IsBoundToObject(this))
    {
        InventoryComponent->OnTrading.AddUObject(this, &UAwesomeHUDWidget::OnTrading);
    }
}

void UAwesomeHUDWidget::InitWidget()
{
    if (DropBoxPosition)
    {
        const auto WidgetToAdd = CreateWidget<UAwesomeDropBoxWidget>(GetOwningPlayer(), DropBoxWidgetClass);
        if (WidgetToAdd)
        {
            DropBoxPosition->AddChild(WidgetToAdd);
        }
    }
    if (InventoriPosition)
    {
        const auto WidgetToAdd = CreateWidget<UInventoryWidget>(GetOwningPlayer(), InventoriWidgetClass);
        if (WidgetToAdd)
        {
            InventoriPosition->AddChild(WidgetToAdd);
        }
    }
    if (EquipmentPosition)
    {
        const auto WidgetToAdd = CreateWidget<UAwesomeEquipmentWidget>(GetOwningPlayer(), EquipmentWidgetClass);
        if (WidgetToAdd)
        {
            EquipmentPosition->AddChild(WidgetToAdd);
        }
    }
    if (PersonalSlotsPosition)
    {
        const auto WidgetToAdd = CreateWidget<UPersonalSlotsWidget>(GetOwningPlayer(), PersonalSlotsWidgetClass);
        if (WidgetToAdd)
        {
            PersonalSlotsPosition->AddChild(WidgetToAdd);
        }
    }
    if (CraftingDeckPosition)
    {
        const auto WidgetToAdd = CreateWidget<UAwesomeCraftingDeckWidget>(GetOwningPlayer(), CraftingDeckWidgetClass);
        if (WidgetToAdd)
        {
            CraftingDeckPosition->AddChild(WidgetToAdd);
        }
    }
    if (ShopPosition)
    {
        const auto WidgetToAdd = CreateWidget<UShopWidget>(GetOwningPlayer(), ShopWidgetClass);
        if (WidgetToAdd)
        {
            ShopPosition->AddChild(WidgetToAdd);
            ShopPosition->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}

void UAwesomeHUDWidget::OnSwitchCraftingPanel()
{
    if (WidgetSwitcherBetween && CraftingPanel)
    {
        WidgetSwitcherBetween->SetActiveWidget(CraftingPanel);
    }
}

void UAwesomeHUDWidget::OnGoBack()
{
    if (WidgetSwitcherBetween && InventoryAndEquipmentPanel)
    {
        WidgetSwitcherBetween->SetActiveWidget(InventoryAndEquipmentPanel);
    }
}

void UAwesomeHUDWidget::OnTrading(bool Enable)
{
    Enable ? ShopPosition->SetVisibility(ESlateVisibility::Visible) : ShopPosition->SetVisibility(ESlateVisibility::Hidden);
    if (SwitchToCraftingButton)
    {
        SwitchToCraftingButton->SetIsEnabled(!Enable);
    }
}