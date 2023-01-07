// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/DropBoxWidget.h"
#include "UI/DragDropItemOperation.h"
#include "Components/InventoryComponent.h"

bool UDropBoxWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    auto DragDropOperation = Cast<UDragDropItemOperation>(InOperation);
    if (!DragDropOperation) return true;

    const auto InventoryComponent = GetOwningPlayerPawn()->FindComponentByClass<UInventoryComponent>();
    if (!InventoryComponent) return true;

    InventoryComponent->MoveItem_OnServer(DragDropOperation->GetSlotData(),              //
                                          DragDropOperation->GetItemFromLocationType(),  //
                                          DragDropOperation->GetFromEquipmentType(),     //
                                          DragDropOperation->GetFromSlotIndex(),         //
                                          ESlotLocationType::Environment,                //
                                          EEquipmentType::NotEquipment,                  //
                                          0);

    return OnDrop(InGeometry, InDragDropEvent, InOperation);
}