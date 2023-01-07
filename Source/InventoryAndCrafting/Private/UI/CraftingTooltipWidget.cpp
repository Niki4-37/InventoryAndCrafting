// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/CraftingTooltipWidget.h"
#include "Components/HorizontalBox.h"
#include "UI/CraftableItemWidget.h"

void UCraftingTooltipWidget::InitWidget(const TArray<FSlot> InRecipe)
{
    if (CraftingRecipeBox)
    {
        for (const auto& CraftingComponent : InRecipe)
        {
            const auto ItemDataPointer = CraftingComponent.DataTableRowHandle.GetRow<FItemData>("");
            if (!ItemDataPointer) continue;
            const auto ItemData = *ItemDataPointer;

            auto CraftableComponentWidget = CreateWidget<UCraftableItemWidget>(GetOwningPlayer(), CraftableItemWidgetClass);
            if (!CraftableComponentWidget) continue;
            CraftableComponentWidget->InitWidget(ItemData.Icon, CraftingComponent.Amount, NAME_None, {});
            CraftingRecipeBox->AddChild(CraftableComponentWidget);
        }
    }
}