// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/CraftingDeckWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "UI/CraftableItemWidget.h"

void UCraftingDeckWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    InitWidget();
}

void UCraftingDeckWidget::InitWidget()
{
    if (!CraftableItemsBox) return;

    CraftableItemsBox->ClearChildren();

    if (!ItemsTable) return;

    auto ItemNames = ItemsTable->GetRowNames();

    int32 SlotIndex{0};
    for (const auto& ItemName : ItemNames)
    {
        const auto ItemDataPointer = ItemsTable->FindRow<FItemData>(ItemName, "", false);
        if (!ItemDataPointer) continue;
        const auto ItemData = *ItemDataPointer;
        if (!ItemData.bCanCraft) continue;

        auto CraftableItemWidget = CreateWidget<UCraftableItemWidget>(GetOwningPlayer(), CraftableItemWidgetClass);
        if (!CraftableItemWidget) continue;
        CraftableItemWidget->InitWidget(ItemData.Icon, ItemData.OutCraftedAmount, ItemName, ItemData.Recipe);
        auto GridObject = CraftableItemsBox->AddChildToUniformGrid(CraftableItemWidget, SlotIndex / SlotsInRow, SlotIndex % SlotsInRow);
        if (!GridObject) continue;
        GridObject->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
        GridObject->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
        ++SlotIndex;
    }
}