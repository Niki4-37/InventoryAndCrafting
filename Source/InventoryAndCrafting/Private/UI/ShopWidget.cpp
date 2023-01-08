// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/ShopWidget.h"
#include "UI/ItemDataWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/Button.h"
#include "Components/InventoryComponent.h"
#include "UI/DragDropItemOperation.h"

void UShopWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (GetOwningPlayer())
    {
        GetOwningPlayer()->GetOnNewPawnNotifier().AddUObject(this, &UShopWidget::OnNewPawn);
        OnNewPawn(GetOwningPlayerPawn());
    }

    if (CloseShopButton)
    {
        CloseShopButton->OnClicked.AddDynamic(this, &UShopWidget::OnCloseShop);
    }
}

void UShopWidget::OnNewPawn(APawn* NewPawn)
{
    if (!NewPawn) return;
    const auto InventoryComponent = NewPawn->FindComponentByClass<UInventoryComponent>();
    if (!InventoryComponent) return;
    if (!InventoryComponent->OnStuffEquiped.IsBoundToObject(this))
    {
        InventoryComponent->OnStuffEquiped.AddUObject(this, &UShopWidget::OnStuffEquiped);
    }
    if (!InventoryComponent->OnSlotChanged.IsBoundToObject(this))
    {
        InventoryComponent->OnSlotChanged.AddUObject(this, &UShopWidget::OnSlotChanged);
    }
}

void UShopWidget::OnStuffEquiped(const TArray<FSlot>& Slots, ESlotLocationType Type)
{
    if (!ShopItemSlots || Type != ESlotLocationType::ShopSlots) return;
    ShopItemSlots->ClearChildren();

    uint8 SlotIndex{0};
    for (const auto& SlotData : Slots)
    {
        auto ItemDataWidget = CreateWidget<UItemDataWidget>(GetOwningPlayer(), ItemDataWidgetClass);
        if (!ItemDataWidget) continue;
        ItemDataWidget->SetDataSlot(SlotData);
        ItemDataWidget->SetItemIndex(SlotIndex);
        ItemDataWidget->SetSlotLocationType(ESlotLocationType::ShopSlots);
        auto GridObject = ShopItemSlots->AddChildToUniformGrid(ItemDataWidget, SlotIndex / SlotsInRow, SlotIndex % SlotsInRow);
        if (!GridObject) continue;
        GridObject->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
        GridObject->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
        ++SlotIndex;
    }
}

void UShopWidget::OnSlotChanged(const FSlot& NewSlotData, const uint8 SlotIndex, ESlotLocationType Type)
{
    if (!ShopItemSlots || Type != ESlotLocationType::ShopSlots) return;
    // auto ItemDataWidget = Cast<UItemDataWidget>(ShopItemSlots->GetChildAt(SlotIndex));
    // if (!ItemDataWidget) return;
    // ItemDataWidget->SetDataSlot(NewSlotData);
}

void UShopWidget::OnCloseShop()
{
    const auto InventoryComponent = GetOwningPlayerPawn()->FindComponentByClass<UInventoryComponent>();
    if (!InventoryComponent) return;
    InventoryComponent->StopTrading_OnServer();
}
