// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/PlayerHUDWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Components/CanvasPanel.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/InventoryComponent.h"
#include "Components/VitalsComponent.h"
#include "UI/CraftingDeckWidget.h"
#include "UI/DropBoxWidget.h"
#include "UI/EquipmentWidget.h"
#include "UI/InventoryWidget.h"
#include "UI/PersonalSlotsWidget.h"
#include "UI/QuickSlotsWidget.h"
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

void UPlayerHUDWidget::ShowInventory(bool bEnabled)
{
    bIsVisible = bEnabled;
    if (bEnabled)
    {
        SwitchToCraftingButton->SetVisibility(ESlateVisibility::Visible);
        DropBoxPosition->SetVisibility(ESlateVisibility::Visible);
        InventoryPosition->SetVisibility(ESlateVisibility::Visible);
        EquipmentPosition->SetVisibility(ESlateVisibility::Visible);
        PersonalSlotsPosition->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        SwitchToCraftingButton->SetVisibility(ESlateVisibility::Collapsed);
        DropBoxPosition->SetVisibility(ESlateVisibility::Collapsed);
        InventoryPosition->SetVisibility(ESlateVisibility::Collapsed);
        EquipmentPosition->SetVisibility(ESlateVisibility::Collapsed);
        PersonalSlotsPosition->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UPlayerHUDWidget::OnNewPawn(APawn* NewPawn)
{
    if (!NewPawn) return;
    const auto InventoryComponent = NewPawn->FindComponentByClass<UInventoryComponent>();
    if (InventoryComponent && !InventoryComponent->OnTrading.IsBoundToObject(this))
    {
        InventoryComponent->OnTrading.AddUObject(this, &UPlayerHUDWidget::OnTrading);
    }

    const auto VitalsComponent = NewPawn->FindComponentByClass<UVitalsComponent>();
    if (VitalsComponent && !VitalsComponent->OnVitalParameterChanged.IsBoundToObject(this))
    {
        VitalsComponent->OnVitalParameterChanged.AddUObject(this, &UPlayerHUDWidget::OnVitalParameterChanged);
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
    if (InventoryPosition)
    {
        const auto WidgetToAdd = CreateWidget<UInventoryWidget>(GetOwningPlayer(), InventoriWidgetClass);
        if (WidgetToAdd)
        {
            InventoryPosition->AddChild(WidgetToAdd);
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
    if (QuickSlotsPosition)
    {
        const auto WidgetToAdd = CreateWidget<UQuickSlotsWidget>(GetOwningPlayer(), QuickSlotsWidgetClass);
        if (WidgetToAdd)
        {
            QuickSlotsPosition->AddChild(WidgetToAdd);
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
        ShowInventory(true);
        DropBoxPosition->SetVisibility(ESlateVisibility::Collapsed);
        SetVisibility(ESlateVisibility::Visible);
        GetOwningPlayer()->SetInputMode(FInputModeGameAndUI());
        GetOwningPlayer()->bShowMouseCursor = true;

        GetOwningPlayerPawn()->DisableInput(GetOwningPlayer());
    }
    else
    {
        ShopPosition->SetVisibility(ESlateVisibility::Hidden);
        if (bIsVisible)
        {
            DropBoxPosition->SetVisibility(ESlateVisibility::Visible);
        }
    }
}

void UPlayerHUDWidget::OnVitalParameterChanged(EVitalParameterType Type, float Persentage)
{
    if (HealthBar && Type == EVitalParameterType::Health)
    {
        HealthBar->SetPercent(Persentage);
    }

    if (ThirstBar && Type == EVitalParameterType::Thirst)
    {
        ThirstBar->SetPercent(1.f - Persentage);
    }

    if (HungerBar && Type == EVitalParameterType::Hunger)
    {
        HungerBar->SetPercent(1.f - Persentage);
    }

    UE_LOG(LogTemp, Display, TEXT("Hunger"));
}
