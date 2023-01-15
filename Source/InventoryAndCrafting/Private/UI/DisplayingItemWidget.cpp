// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/DisplayingItemWidget.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/InventoryComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UI/DragDropItemOperation.h"

FReply UDisplayingItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (GetOwningPlayerPawn())
    {
        const auto InventoryComponent = GetOwningPlayerPawn()->FindComponentByClass<UInventoryComponent>();
        if (InventoryComponent)
        {
            InventoryComponent->RemoveFromQuickSlots_OnClient(QuickSlotData.FromSlotType, QuickSlotData.FromSlotIndex, true);
            SetDisplayingInfo(nullptr);
        }
    }
    return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

bool UDisplayingItemWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    auto DragDropOperation = Cast<UDragDropItemOperation>(InOperation);
    if (!DragDropOperation) return true;

    if (QuickSlotData.FromSlotType != ESlotLocationType::Default) return true;

    if (DragDropOperation->GetItemFromLocationType() == ESlotLocationType::Inventory || DragDropOperation->GetItemFromLocationType() == ESlotLocationType::PersonalSlots)
    {

        const auto ItemDataPointer = DragDropOperation->GetSlotData().DataTableRowHandle.GetRow<FItemData>("");
        if (ItemDataPointer)
        {
            SetDisplayingInfo(ItemDataPointer->Icon);
        }

        const auto InventoryComponent = GetOwningPlayerPawn()->FindComponentByClass<UInventoryComponent>();
        if (!InventoryComponent) return false;

        InventoryComponent->AddToQuickSlotsAtIndex(FQuickSlot(DragDropOperation->GetItemFromLocationType(), DragDropOperation->GetFromSlotIndex()), WidgetIndex);
        return true;
    }

    return true;
}

void UDisplayingItemWidget::SetDisplayingInfo(UTexture2D* NewIcon, int32 InAmount)
{
    if (DisplayingBorder && EmptyIcon)
    {
        NewIcon ? DisplayingBorder->SetBrushFromTexture(NewIcon) : DisplayingBorder->SetBrushFromTexture(EmptyIcon);
    }

    if (AmountText)
    {
        AmountText->SetText(FText::AsNumber(InAmount));
        InAmount > 0 ? AmountText->SetVisibility(ESlateVisibility::Visible) : AmountText->SetVisibility(ESlateVisibility::Hidden);
    }

    if (KeyboardKeyText)
    {
        KeyboardKeyText->SetText(FText::AsNumber(WidgetIndex + 1));
    }
}