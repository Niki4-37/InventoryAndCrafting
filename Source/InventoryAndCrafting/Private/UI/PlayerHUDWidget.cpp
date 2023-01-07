// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/PlayerHUDWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Components/CanvasPanel.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/InventoryComponent.h"
#include "UI/CraftingDeckWidget.h"
#include "UI/DropBoxWidget.h"
#include "UI/EquipmentWidget.h"
#include "UI/InventoryWidget.h"
#include "UI/PersonalSlotsWidget.h"
#include "UI/ShopWidget.h"

void UPlayerHUDWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (SwitchToCraftingButton)
    {
        SwitchToCraftingButton->OnClicked.AddDynamic(this, &UPlayerHUDWidget::OnSwitchCraftingPanel);
    }
    if (BackButton)
    {
        BackButton->OnClicked.AddDynamic(this, &UPlayerHUDWidget::OnGoBack);
    }
    if (WidgetSwitcherBetween && InventoryAndEquipmentPanel)
    {
        WidgetSwitcherBetween->SetActiveWidget(InventoryAndEquipmentPanel);
    }
    if (GetOwningPlayer())
    {
        GetOwningPlayer()->GetOnNewPawnNotifier().AddUObject(this, &UPlayerHUDWidget::OnNewPawn);
        OnNewPawn(GetOwningPlayerPawn());
    }

    InitWidget();
}

void UPlayerHUDWidget::OnNewPawn(APawn* NewPawn)
{
    if (!NewPawn) return;
    const auto InventoryComponent = NewPawn->FindComponentByClass<UInventoryComponent>();
    if (!InventoryComponent) return;
    if (!InventoryComponent->OnTrading.IsBoundToObject(this))
    {
        InventoryComponent->OnTrading.AddUObject(this, &UPlayerHUDWidget::OnTrading);
    }
}

void UPlayerHUDWidget::InitWidget()
{
    if (DropBoxPosition)
    {
        const auto WidgetToAdd = CreateWidget<UDropBoxWidget>(GetOwningPlayer(), DropBoxWidgetClass);
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
        const auto WidgetToAdd = CreateWidget<UEquipmentWidget>(GetOwningPlayer(), EquipmentWidgetClass);
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
        const auto WidgetToAdd = CreateWidget<UCraftingDeckWidget>(GetOwningPlayer(), CraftingDeckWidgetClass);
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

void UPlayerHUDWidget::OnSwitchCraftingPanel()
{
    if (WidgetSwitcherBetween && CraftingPanel)
    {
        WidgetSwitcherBetween->SetActiveWidget(CraftingPanel);
    }
}

void UPlayerHUDWidget::OnGoBack()
{
    if (WidgetSwitcherBetween && InventoryAndEquipmentPanel)
    {
        WidgetSwitcherBetween->SetActiveWidget(InventoryAndEquipmentPanel);
    }
}

void UPlayerHUDWidget::OnTrading(bool Enable)
{
    if (SwitchToCraftingButton)
    {
        SwitchToCraftingButton->SetIsEnabled(!Enable);
    }

    if (Enable)
    {
        ShopPosition->SetVisibility(ESlateVisibility::Visible);
        SetVisibility(ESlateVisibility::Visible);
        GetOwningPlayer()->SetInputMode(FInputModeGameAndUI());
        GetOwningPlayer()->bShowMouseCursor = true;

        GetOwningPlayerPawn()->DisableInput(GetOwningPlayer());
    }
    else
    {
        ShopPosition->SetVisibility(ESlateVisibility::Hidden);
    }
}