// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/CraftingTooltipWidget.h"
#include "Components/HorizontalBox.h"
#include "UI/ItemDataWidget.h"

void UCraftingTooltipWidget::InitWidget(const TArray<FSlot> InRecipe)
{
    if (CraftingRecipeBox)
    {
        for (const auto& CraftingComponent : InRecipe)
        {
            const auto ItemDataPointer = CraftingComponent.DataTableRowHandle.GetRow<FItemData>("");
            if (!ItemDataPointer) continue;
            const auto ItemData = *ItemDataPointer;

            auto CraftableComponentWidget = CreateWidget<UItemDataWidget>(GetOwningPlayer(), ItemDataWidgetClass);
            if (!CraftableComponentWidget) continue;
            CraftableComponentWidget->SetDataSlot(CraftingComponent);
            CraftingRecipeBox->AddChild(CraftableComponentWidget);
        }
    }
}