// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/AwesomeItemDataWidget.h"
#include "Components/Border.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UI/AwesomeDragDropItemOperation.h"

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
    // Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent); //??
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
