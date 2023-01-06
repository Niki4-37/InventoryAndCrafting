// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/ConfirmWidget.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/InventoryComponent.h"

void UConfirmWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (ConfirmButton)
    {
        ConfirmButton->OnClicked.AddDynamic(this, &UConfirmWidget::OnCorfirm);
    }
    if (CancelButton)
    {
        CancelButton->OnClicked.AddDynamic(this, &UConfirmWidget::OnCancel);
    }
}

void UConfirmWidget::SetSlotData(const FSlot& Data)
{
    SlotData = Data;

    const auto PickupDataPointer = SlotData.DataTableRowHandle.GetRow<FItemData>("");
    if (!PickupDataPointer || !PickupDataPointer->Icon || !IconBorder) return;

    IconBorder->SetBrushFromTexture(PickupDataPointer->Icon);
}

void UConfirmWidget::OnCorfirm()
{
    const auto InventoryComponent = GetOwningPlayerPawn()->FindComponentByClass<UInventoryComponent>();
    if (!InventoryComponent) return;

    InventoryComponent->MoveItem_OnServer(SlotData,                        //
                                          DragDropData.FromLocationType,   //
                                          DragDropData.FromEquipmentType,  //
                                          DragDropData.FromSlotIndex,      //
                                          DragDropData.ToLocationType,     //
                                          DragDropData.ToEquipmentType,    //
                                          DragDropData.ToSlotIndex);

    RemoveFromParent();
}

void UConfirmWidget::OnCancel()
{
    RemoveFromParent();
}