// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/AwesomeDropBoxWidget.h"
#include "UI/AwesomeDragDropItemOperation.h"
#include "Player/AwesomeBaseCharacter.h"
#include "Player/AwesomePlayerController.h"
#include "Pickup/AwesomeBackpackMaster.h"

bool UAwesomeDropBoxWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    auto DragDropOperation = Cast<UAwesomeDragDropItemOperation>(InOperation);
    if (!DragDropOperation) return true;

    switch (DragDropOperation->GetSlotData().ItemLocationType)
    {
        case EItemLocationType::Inventory:
        {
            DropFromInventory(DragDropOperation->GetSlotIndex(), DragDropOperation->GetSlotData());
            break;
        }
        case EItemLocationType::Equipment:
        {
            DropFromEquipment(DragDropOperation->GetSlotIndex(), DragDropOperation->GetSlotData());
            break;
        }
    }

    return OnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UAwesomeDropBoxWidget::DropFromInventory(const uint8 Index, const FSlot& DroppedItem)
{
    const auto Player = Cast<AAwesomeBaseCharacter>(GetOwningPlayerPawn());
    if (!Player) return;

    if (!Player->GetBackpack()) return;
    if (Player->GetBackpack()->RemoveAmountFromInventorySlotsAtIndex(Index, DroppedItem.Amount))
    {
        if (const auto AwesomePlayerController = Cast<AAwesomePlayerController>(GetOwningPlayer()))
        {
            AwesomePlayerController->SpawnDroppedItem(DroppedItem);
        }
    }
}

void UAwesomeDropBoxWidget::DropFromEquipment(const uint8 Index, const FSlot& DroppedItem)
{
    const auto Player = Cast<AAwesomeBaseCharacter>(GetOwningPlayerPawn());
    if (!Player) return;

    if (Player->RemoveAmountFromEquipmentSlotsAtIndex(Index, DroppedItem.Amount))
    {
        if (const auto AwesomePlayerController = Cast<AAwesomePlayerController>(GetOwningPlayer()))
        {
            AwesomePlayerController->SpawnDroppedItem(DroppedItem);
        }
    }
}