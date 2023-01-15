// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/QuickSlotsWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/InventoryComponent.h"
#include "UI/DisplayingItemWidget.h"

void UQuickSlotsWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (GetOwningPlayer())
    {
        GetOwningPlayer()->GetOnNewPawnNotifier().AddUObject(this, &UQuickSlotsWidget::OnNewPawn);
        OnNewPawn(GetOwningPlayerPawn());
    }
}

void UQuickSlotsWidget::OnNewPawn(APawn* NewPawn)
{
    if (!NewPawn) return;
    const auto InventoryComponent = NewPawn->FindComponentByClass<UInventoryComponent>();
    if (!InventoryComponent) return;
    if (!InventoryComponent->OnQuickSlotsCreated.IsBoundToObject(this))
    {
        InventoryComponent->OnQuickSlotsCreated.AddUObject(this, &UQuickSlotsWidget::OnQuickSlotsCreated);
    }
    if (!InventoryComponent->OnQuickSlotsDataChanged.IsBoundToObject(this))
    {
        InventoryComponent->OnQuickSlotsDataChanged.AddUObject(this, &UQuickSlotsWidget::OnQuickSlotsDataChanged);
    }
}

void UQuickSlotsWidget::OnQuickSlotsCreated(const TArray<FQuickSlot>& Slots)
{
    if (!QuickSlots) return;
    QuickSlots->ClearChildren();

    uint8 SlotIndex{0};
    for (const auto& SlotData : Slots)
    {
        const auto ItemDataWidget = CreateWidget<UDisplayingItemWidget>(GetOwningPlayer(), DisplayingItemWidgetClass);
        if (!ItemDataWidget) continue;
        ItemDataWidget->SetQuickSlotData(SlotData);
        ItemDataWidget->SetWidgetIndex(SlotIndex);
        ItemDataWidget->SetDisplayingInfo(nullptr);
        auto GridObject = QuickSlots->AddChildToUniformGrid(ItemDataWidget, SlotIndex / SlotsInRow, SlotIndex % SlotsInRow);
        if (!GridObject) continue;
        GridObject->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
        GridObject->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
        ++SlotIndex;
    }
}

void UQuickSlotsWidget::OnQuickSlotsDataChanged(const FQuickSlot& Data, uint8 Index)
{
    if (!QuickSlots) return;
    auto ItemDataWidget = Cast<UDisplayingItemWidget>(QuickSlots->GetChildAt(Index));
    if (!ItemDataWidget) return;
    ItemDataWidget->SetQuickSlotData(Data);
    if (Data.FromSlotType == ESlotLocationType::Default)
    {
        ItemDataWidget->SetDisplayingInfo(nullptr);
    }
}
