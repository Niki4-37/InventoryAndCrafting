// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/AwesomeItemDataWidget.h"
#include "Components/Border.h"

void UAwesomeItemDataWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
}

void UAwesomeItemDataWidget::SetDataFromItem(const FSlot& ItemData)
{
    Item = ItemData;

    if (!Item.DataTableRowHandle.DataTable) return;
    const auto PickupDataPointer = Item.DataTableRowHandle.DataTable->FindRow<FInventoryData>(Item.DataTableRowHandle.RowName, "", false);
    if (!PickupDataPointer) return;
    InventoryData = *PickupDataPointer;
    SetIconToWidget();
}

void UAwesomeItemDataWidget::SetIconToWidget()
{
    if (!WidgetBorder || !InventoryData.Icon) return;
    WidgetBorder->SetBrushFromTexture(InventoryData.Icon);
}
