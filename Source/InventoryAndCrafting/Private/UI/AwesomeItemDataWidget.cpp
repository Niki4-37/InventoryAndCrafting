// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/AwesomeItemDataWidget.h"
#include "Components/Border.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UI/AwesomeDragDropItemOperation.h"
#include "Player/AwesomeBaseCharacter.h"
#include "Pickup/AwesomeBackpackMaster.h"

void UAwesomeItemDataWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    SetIconToWidget();
}

void UAwesomeItemDataWidget::SetDataFromSlot(const FSlot& InSlotData)
{
    SlotData = InSlotData;

    if (!SlotData.DataTableRowHandle.DataTable) return;
    const auto PickupDataPointer = SlotData.DataTableRowHandle.DataTable->FindRow<FItemData>(SlotData.DataTableRowHandle.RowName, "", false);
    if (!PickupDataPointer) return;
    ItemData = *PickupDataPointer;
    SetIconToWidget();
}

FReply UAwesomeItemDataWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

void UAwesomeItemDataWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    auto DragDrop = Cast<UAwesomeDragDropItemOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UAwesomeDragDropItemOperation::StaticClass()));
    if (DragDrop)
    {
        DragDrop->SetSlotData(FSlot(SlotData.DataTableRowHandle, 1, SlotData.ItemLocationType));
        DragDrop->SetSlotIndex(ItemIndex);
        DragDrop->DefaultDragVisual = this;
        DragDrop->Pivot = EDragPivot::CenterCenter;
    }
    OutOperation = DragDrop;
    Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
}

bool UAwesomeItemDataWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    auto DragDropOperation = Cast<UAwesomeDragDropItemOperation>(InOperation);
    if (!DragDropOperation) return true;

    const auto Player = Cast<AAwesomeBaseCharacter>(GetOwningPlayerPawn());
    if (!Player || !Player->GetBackpack()) return true;

    switch (DragDropOperation->GetSlotData().ItemLocationType)
    {
        case EItemLocationType::Inventory:
        {
            if (Player->TryAddItemToEquipmentSlotsByIndex(DragDropOperation->GetSlotData(), ItemIndex))
            {
                Player->GetBackpack()->RemoveAmountFromInventorySlotsAtIndex(DragDropOperation->GetSlotIndex(), DragDropOperation->GetSlotData().Amount);
            }
            break;
        }
        case EItemLocationType::Equipment:
        {
            if (Player->GetBackpack()->TryAddItemToSlots(DragDropOperation->GetSlotData()))
            {
                Player->RemoveAmountFromEquipmentSlotsAtIndex(DragDropOperation->GetSlotIndex(), DragDropOperation->GetSlotData().Amount);
            }
            break;
        }
    }

    return OnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UAwesomeItemDataWidget::SetIconToWidget()
{
    if (!WidgetBorder) return;

    if (!SlotData.Amount && EmptyIcon)
    {
        WidgetBorder->SetBrushFromTexture(EmptyIcon);
        return;
    }

    if (!ItemData.Icon) return;
    WidgetBorder->SetBrushFromTexture(ItemData.Icon);
}
