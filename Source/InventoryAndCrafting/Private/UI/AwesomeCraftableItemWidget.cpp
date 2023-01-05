// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/AwesomeCraftableItemWidget.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Player/AwesomeBaseCharacter.h"
#include "Player/AwesomePlayerController.h"
#include "Pickup/AwesomeBackpackMaster.h"

void UAwesomeCraftableItemWidget::InitWidget(UTexture2D* Icon, int32 Amount, const FName& InItemName, const TArray<FSlot> InRecipe)
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

FReply UAwesomeCraftableItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    CraftTheItem();
    return OnMouseButtonDown(InGeometry, InMouseEvent).NativeReply;
}

bool UAwesomeCraftableItemWidget::CheckEnabledCraftableComponents(AAwesomeBaseCharacter* Player)
{
    // if (!Player || !Player->GetBackpack()) return false;

    // for (const auto& CraftableComponent : Recipe)
    //{
    //     uint8 SlotIndex;
    //     int32 FoundStackAmount;
    //     bool OutCanStack;
    //     if (!Player->GetBackpack()->FindStackOfSameItems(CraftableComponent, SlotIndex, FoundStackAmount, OutCanStack)) return false;
    //     if (FoundStackAmount < CraftableComponent.Amount) return false;
    //     FCraftingSet CraftingSet(SlotIndex, CraftableComponent.Amount);
    //     ComponentsFromRecipe.Add(CraftingSet);
    // }

    return true;
}

void UAwesomeCraftableItemWidget::CraftTheItem()
{
    const auto Player = Cast<AAwesomeBaseCharacter>(GetOwningPlayerPawn());
    // if (!Player || !Player->GetBackpack()) return;

    // if (CheckEnabledCraftableComponents(Player))
    //{
    //     FDataTableRowHandle DataTableRowHandle;
    //     DataTableRowHandle.DataTable = ItemsTable; /* must be set in blueprint! */
    //     DataTableRowHandle.RowName = ItemName;
    //     if (Player->GetBackpack()->TryAddItemToSlots(FSlot(DataTableRowHandle, CraftingOutAmount)))
    //     {
    //         SpendComponents(Player->GetBackpack());
    //         return;
    //     }
    //     else
    //     {
    //         const auto PlayerController = Cast<AAwesomePlayerController>(GetOwningPlayer());
    //         if (!PlayerController) return;
    //         PlayerController->SpawnDroppedItem(FSlot(DataTableRowHandle, CraftingOutAmount));
    //         SpendComponents(Player->GetBackpack());
    //         return;
    //     }
    // }
}

void UAwesomeCraftableItemWidget::SpendComponents(AAwesomeBackpackMaster* Backpack)
{
    if (ComponentsFromRecipe.Num() == 0 || !Backpack) return;
    for (auto Component : ComponentsFromRecipe)
    {
        Backpack->RemoveAmountFromInventorySlotsAtIndex(Component.ComponentIndex, Component.ComponentValue);
    }
    ComponentsFromRecipe.Empty();
}
