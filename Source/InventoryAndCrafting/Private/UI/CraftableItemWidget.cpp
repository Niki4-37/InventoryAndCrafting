// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/CraftableItemWidget.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/InventoryComponent.h"
#include "Pickup/AwesomeBackpackMaster.h"

void UCraftableItemWidget::InitWidget(UTexture2D* Icon, int32 Amount, const FName& InItemName, const TArray<FSlot> InRecipe)
{
    CraftingOutAmount = Amount;
    Recipe = InRecipe;
    ItemName = InItemName;

    if (CraftingBorder && Icon)
    {
        CraftingBorder->SetBrushFromTexture(Icon);
    }

    if (OutCraftingAmountText)
    {
        OutCraftingAmountText->SetText(FText::FromString(FString::FromInt(Amount)));
    }
}

FReply UCraftableItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    CraftTheItem();
    return OnMouseButtonDown(InGeometry, InMouseEvent).NativeReply;
}

bool UCraftableItemWidget::CheckForCraftingComponents(UInventoryComponent* Inventory)
{
    if (!Inventory || !Inventory->GetBackpack()) return false;

    for (const auto& CraftableComponent : Recipe)
    {
        uint8 SlotIndex;
        int32 FoundStackAmount;
        bool OutCanStack;
        if (!Inventory->GetBackpack()->FindStackOfSameItems(CraftableComponent, SlotIndex, FoundStackAmount, OutCanStack)) return false;
        if (FoundStackAmount < CraftableComponent.Amount) return false;
        FCraftingSet CraftingSet(SlotIndex, CraftableComponent.Amount);
        ComponentsFromRecipe.Add(CraftingSet);
    }

    return true;
}

void UCraftableItemWidget::CraftTheItem()
{
    if (!GetOwningPlayerPawn()) return;
    const auto InventoryComonent = GetOwningPlayerPawn()->FindComponentByClass<UInventoryComponent>();
    if (!InventoryComonent || !InventoryComonent->GetBackpack()) return;
    UE_LOG(LogTemp, Display, TEXT("CraftTheItem"));
    if (CheckForCraftingComponents(InventoryComonent))
    {
        FDataTableRowHandle DataTableRowHandle;
        DataTableRowHandle.DataTable = ItemsTable; /* must be set in blueprint! */
        DataTableRowHandle.RowName = ItemName;
        if (InventoryComonent->GetBackpack()->TryAddItemToSlots(FSlot(DataTableRowHandle, CraftingOutAmount)))
        {
            SpendComponents(InventoryComonent->GetBackpack());
            return;
        }
        else
        {
            InventoryComonent->DropItem(FSlot(DataTableRowHandle, CraftingOutAmount));
            SpendComponents(InventoryComonent->GetBackpack());
            return;
        }
    }
}

void UCraftableItemWidget::SpendComponents(AAwesomeBackpackMaster* Backpack)
{
    if (ComponentsFromRecipe.Num() == 0 || !Backpack) return;
    for (auto Component : ComponentsFromRecipe)
    {
        Backpack->RemoveAmountFromInventorySlotsAtIndex(Component.ComponentIndex, Component.ComponentValue);
    }
    ComponentsFromRecipe.Empty();
}
