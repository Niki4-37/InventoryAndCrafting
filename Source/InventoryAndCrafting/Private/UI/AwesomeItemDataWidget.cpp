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

void UAwesomeItemDataWidget::SetDataSlot(const FSlot& InSlotData)
{
    SlotData = InSlotData;
    if (!SlotData.Amount)
    {
        SetIconToWidget();
        return;
    }
    const auto PickupDataPointer = SlotData.DataTableRowHandle.GetRow<FItemData>("");
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

    const auto Player = Cast<AAwesomeBaseCharacter>(GetOwningPlayerPawn());
    if (!Player) return true;

    Player->MoveItem_OnServer(DragDropOperation->GetSlotData(),              //
                              DragDropOperation->GetItemFromLocationType(),  //
                              DragDropOperation->GetFromEquipmentType(),     //
                              DragDropOperation->GetFromSlotIndex(),         //
                              LocationType,                                  //
                              EquipmentType,                                 //
                              ItemIndex);

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
