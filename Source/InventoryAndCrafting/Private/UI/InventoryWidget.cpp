// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/InventoryWidget.h"
#include "UI/ItemDataWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/TextBlock.h"
#include "Components/InventoryComponent.h"

void UInventoryWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (GetOwningPlayer())
    {
        GetOwningPlayer()->GetOnNewPawnNotifier().AddUObject(this, &UInventoryWidget::OnNewPawn);
        OnNewPawn(GetOwningPlayerPawn());
    }
}

void UInventoryWidget::OnNewPawn(APawn* NewPawn)
{
    if (!NewPawn) return;
    const auto InventoryComponent = NewPawn->FindComponentByClass<UInventoryComponent>();
    if (!InventoryComponent) return;
    if (!InventoryComponent->OnStuffEquiped.IsBoundToObject(this))
    {
        InventoryComponent->OnStuffEquiped.AddUObject(this, &UInventoryWidget::OnStuffEquiped);
    }
    if (!InventoryComponent->OnSlotChanged.IsBoundToObject(this))
    {
        InventoryComponent->OnSlotChanged.AddUObject(this, &UInventoryWidget::OnSlotChanged);
    }
}

void UInventoryWidget::OnStuffEquiped(const TArray<FSlot>& Slots, ESlotLocationType Type)
{
    if (!InventoryItemSlots || Type != ESlotLocationType::Inventory) return;
    InventoryItemSlots->ClearChildren();

    if (NoBackpackText)
    {
        Slots.Num() ? NoBackpackText->SetText(FText::FromString("")) : NoBackpackText->SetText(FText::FromString("NoBackpack"));
    }

    uint8 SlotIndex{0};
    for (const auto& SlotData : Slots)
    {
        auto ItemDataWidget = CreateWidget<UItemDataWidget>(GetOwningPlayer(), ItemDataWidgetClass);
        if (!ItemDataWidget) continue;
        ItemDataWidget->SetDataSlot(SlotData);
        ItemDataWidget->SetItemIndex(SlotIndex);
        ItemDataWidget->SetSlotLocationType(ESlotLocationType::Inventory);
        auto GridObject = InventoryItemSlots->AddChildToUniformGrid(ItemDataWidget, SlotIndex / SlotsInRow, SlotIndex % SlotsInRow);
        if (!GridObject) continue;
        GridObject->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
        GridObject->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
        ++SlotIndex;
    }
}

void UInventoryWidget::OnSlotChanged(const FSlot& NewSlotData, const uint8 SlotIndex, ESlotLocationType Type)
{
    if (!InventoryItemSlots || Type != ESlotLocationType::Inventory) return;
    auto ItemDataWidget = Cast<UItemDataWidget>(InventoryItemSlots->GetChildAt(SlotIndex));
    if (!ItemDataWidget) return;
    ItemDataWidget->SetDataSlot(NewSlotData);
}
