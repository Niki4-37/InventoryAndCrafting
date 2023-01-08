// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/ItemDataWidget.h"
#include "Components/Border.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UI/DragDropItemOperation.h"
#include "UI/ConfirmWidget.h"
#include "Components/InventoryComponent.h"

void UItemDataWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    SetIconToWidget(nullptr);
}

void UItemDataWidget::SetDataSlot(const FSlot& InSlotData)
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

FReply UItemDataWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

void UItemDataWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    if (!SlotData.Amount) return;
    auto DragDrop = Cast<UDragDropItemOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropItemOperation::StaticClass()));
    if (DragDrop)
    {
        DragDrop->SetSlotData(FSlot(SlotData.DataTableRowHandle, 1));
        DragDrop->SetFromSlotIndex(ItemIndex);
        DragDrop->SetFromSlotLocationType(LocationType);
        DragDrop->SetFromEquipmentType(EquipmentType);

        auto DradDropWidget = CreateWidget<UItemDataWidget>(GetOwningPlayer(), ItemDataWidgetClass);
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

bool UItemDataWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    auto DragDropOperation = Cast<UDragDropItemOperation>(InOperation);
    if (!DragDropOperation) return true;

    if (DragDropOperation->GetItemFromLocationType() == ESlotLocationType::ShopSlots || LocationType == ESlotLocationType::ShopSlots)
    {
        auto ConfirmWidget = CreateWidget<UConfirmWidget>(GetOwningPlayer(), ConfirmWidgetClass);
        if (ConfirmWidget)
        {
            ConfirmWidget->SetSlotData(DragDropOperation->GetSlotData());
            ConfirmWidget->SetDraDropData(FDragDropData(DragDropOperation->GetItemFromLocationType(),  //
                                                        DragDropOperation->GetFromEquipmentType(),     //
                                                        DragDropOperation->GetFromSlotIndex(),         //
                                                        LocationType,                                  //
                                                        EquipmentType,                                 //
                                                        ItemIndex));
            ConfirmWidget->AddToViewport();

            return OnDrop(InGeometry, InDragDropEvent, InOperation);
        }
    }

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

void UItemDataWidget::SetIconToWidget(UTexture2D* NewIcon)
{
    if (!WidgetBorder || !EmptyIcon) return;

    NewIcon ? WidgetBorder->SetBrushFromTexture(NewIcon) : WidgetBorder->SetBrushFromTexture(EmptyIcon);
}
