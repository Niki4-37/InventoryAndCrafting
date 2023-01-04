// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/AwesomeDropBoxWidget.h"
#include "UI/AwesomeDragDropItemOperation.h"
#include "Player/AwesomeBaseCharacter.h"
//#include "Player/AwesomePlayerController.h"
//#include "Pickup/AwesomeBackpackMaster.h"

bool UAwesomeDropBoxWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    auto DragDropOperation = Cast<UAwesomeDragDropItemOperation>(InOperation);
    if (!DragDropOperation) return true;

    const auto Player = Cast<AAwesomeBaseCharacter>(GetOwningPlayerPawn());
    if (!Player) return true;

    Player->MoveItem_OnServer(DragDropOperation->GetSlotData(),              //
                              DragDropOperation->GetItemFromLocationType(),  //
                              DragDropOperation->GetFromEquipmentType(),     //
                              DragDropOperation->GetFromSlotIndex(),         //
                              ESlotLocationType::Environment,                //
                              EEquipmentType::NotEquipment,                  //
                              0);

    return OnDrop(InGeometry, InDragDropEvent, InOperation);
}