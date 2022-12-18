// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/AwesomeCraftingTooltipWidget.h"
#include "Components/HorizontalBox.h"
#include "UI/AwesomeCraftableItemWidget.h"

void UAwesomeCraftingTooltipWidget::InitWidget(const TArray<FSlot> InRecipe)
{
    if (CraftingRecipeBox)
    {
        for (const auto& CraftingComponent : InRecipe)
        {
            // if (!CraftableComponent.DataTableRowHandle.DataTable) continue;
            //  const auto ItemDataPointer = CraftableComponent.DataTableRowHandle.DataTable->FindRow<FItemData>(CraftableComponent.DataTableRowHandle.RowName, "", false);
            const auto ItemDataPointer = CraftingComponent.DataTableRowHandle.GetRow<FItemData>("");
            if (!ItemDataPointer) continue;
            const auto ItemData = *ItemDataPointer;

            auto CraftableComponentWidget = CreateWidget<UAwesomeCraftableItemWidget>(GetOwningPlayer(), CraftableItemWidgetClass);
            if (!CraftableComponentWidget) continue;
            CraftableComponentWidget->InitWidget(ItemData.Icon, CraftingComponent.Amount, NAME_None, {});
            CraftingRecipeBox->AddChild(CraftableComponentWidget);
        }
    }
}