// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/AwesomeInventoryWidget.h"
#include "Player/AwesomeBaseCharacter.h"
#include "UI/AwesomeItemDataWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Pickup/AwesomeBackpackMaster.h"

void UAwesomeInventoryWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (GetOwningPlayer())
    {
        GetOwningPlayer()->GetOnNewPawnNotifier().AddUObject(this, &UAwesomeInventoryWidget::OnNewPawn);
        OnNewPawn(GetOwningPlayerPawn());
    }
}

void UAwesomeInventoryWidget::OnNewPawn(APawn* NewPawn)
{
    const auto Player = Cast<AAwesomeBaseCharacter>(NewPawn);
    if (!Player) return;
    if (!Player->OnStuffEquiped.IsBoundToObject(this))
    {
        Player->OnStuffEquiped.AddUObject(this, &UAwesomeInventoryWidget::OnStuffEquiped);
    }
    if (!Player->OnSlotChanged.IsBoundToObject(this))
    {
        Player->OnSlotChanged.AddUObject(this, &UAwesomeInventoryWidget::OnSlotChanged);
    }
}

void UAwesomeInventoryWidget::OnStuffEquiped(const TArray<FSlot>& Slots, ESlotLocationType Type)
{
    if (!InventoryItemSlots || Type != ESlotLocationType::Inventory) return;
    InventoryItemSlots->ClearChildren();

    uint8 SlotIndex{0};
    for (const auto& SlotData : Slots)
    {
        auto ItemDataWidget = CreateWidget<UAwesomeItemDataWidget>(GetOwningPlayer(), ItemDataWidgetClass);
        if (!ItemDataWidget) continue;
        ItemDataWidget->SetDataSlot(SlotData);
        ItemDataWidget->SetItemIndex(SlotIndex);
        ItemDataWidget->SetSlotLocationType(ESlotLocationType::Inventory);
        auto GridObject = InventoryItemSlots->AddChildToUniformGrid(ItemDataWidget, SlotIndex / SlotsInRow, SlotIndex % SlotsInRow);
        if (!GridObject) continue;
        GridObject->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
        GridObject->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
        ++SlotIndex;
    }
}

void UAwesomeInventoryWidget::OnSlotChanged(const FSlot& NewSlotData, const uint8 SlotIndex, ESlotLocationType Type)
{
    if (!InventoryItemSlots || Type != ESlotLocationType::Inventory) return;
    auto ItemDataWidget = Cast<UAwesomeItemDataWidget>(InventoryItemSlots->GetChildAt(SlotIndex));
    if (!ItemDataWidget) return;
    ItemDataWidget->SetDataSlot(NewSlotData);
}
