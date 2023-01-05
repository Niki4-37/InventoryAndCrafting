// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/AwesomeItemDataWidget.h"
#include "Components/Border.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UI/AwesomeDragDropItemOperation.h"
#include "Components/InventoryComponent.h"

void UAwesomeItemDataWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    SetIconToWidget(nullptr);
}

void UAwesomeItemDataWidget::SetDataSlot(const FSlot& InSlotData)
{
    SlotData = InSlotData;
    if (!SlotData.Amount)
    {
        SetIconToWidget(nullptr);
        return;
    }
    const auto PickupDataPointer = SlotData.DataTableRowHandle.GetRow<FItemData>("");
    if (!PickupDataPointer) return;
    ItemData = *PickupDataPointer;
    SetIconToWidget(ItemData.Icon);
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
        DragDrop->SetSlotData(FSlot(SlotData.DataTableRowHandle, 1));
        DragDrop->SetFromSlotIndex(ItemIndex);
        DragDrop->SetFromSlotLocationType(LocationType);
        DragDrop->SetFromEquipmentType(EquipmentType);
        auto DradDropWidget = CreateWidget<UAwesomeItemDataWidget>(GetOwningPlayer(), ItemDataWidgetClass);
        if (DradDropWidget)
        {
            DradDropWidget->SetDataSlot(SlotData);
            DradDropWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
            DragDrop->DefaultDragVisual = DradDropWidget;
        }
        DragDrop->Pivot = EDragPivot::CenterCenter;
    }
    OutOperation = DragDrop;
    Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
}

bool UAwesomeItemDataWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    auto DragDropOperation = Cast<UAwesomeDragDropItemOperation>(InOperation);
    if (!DragDropOperation) return true;

    const auto InventoryComponent = GetOwningPlayerPawn()->FindComponentByClass<UInventoryComponent>();
    if (!InventoryComponent) return true;

    InventoryComponent->MoveItem_OnServer(DragDropOperation->GetSlotData(),              //
                                          DragDropOperation->GetItemFromLocationType(),  //
                                          DragDropOperation->GetFromEquipmentType(),     //
                                          DragDropOperation->GetFromSlotIndex(),         //
                                          LocationType,                                  //
                                          EquipmentType,                                 //
                                          ItemIndex);

    return OnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UAwesomeItemDataWidget::SetIconToWidget(UTexture2D* NewIcon)
{
    if (!WidgetBorder || !EmptyIcon) return;

    NewIcon ? WidgetBorder->SetBrushFromTexture(NewIcon) : WidgetBorder->SetBrushFromTexture(EmptyIcon);
}
