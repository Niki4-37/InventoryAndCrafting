// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/AwesomeInventoryWidget.h"
#include "Player/AwesomeBaseCharacter.h"
#include "UI/AwesomeItemDataWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"

void UAwesomeInventoryWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (GetOwningPlayer())
    {
        GetOwningPlayer()->GetOnNewPawnNotifier().AddUObject(this, &UAwesomeInventoryWidget::OnNewPawn);
        OnNewPawn(GetOwningPlayerPawn());
    }
}

void UAwesomeInventoryWidget::OnNewPawn(APawn* NewPawn)
{
    const auto Player = Cast<AAwesomeBaseCharacter>(NewPawn);
    if (!Player) return;

    Player->OnItemPickedup.AddUObject(this, &UAwesomeInventoryWidget::OnItemPickedup);

    UpdateItemSlots(Player->GetSlots());
}

void UAwesomeInventoryWidget::OnItemPickedup(const TArray<FSlot>& Slots)
{
    UpdateItemSlots(Slots);
}

void UAwesomeInventoryWidget::UpdateItemSlots(const TArray<FSlot>& Slots)
{
    if (!InventoryItemSlots) return;
    InventoryItemSlots->ClearChildren();

    uint8 SlotIndex{0};
    for (const auto& SlotData : Slots)
    {
        auto ItemDataWidget = CreateWidget<UAwesomeItemDataWidget>(GetOwningPlayer(), ItemDataWidgetClass);
        if (!ItemDataWidget) continue;
        ItemDataWidget->SetDataFromItem(SlotData);
        auto GridObject = InventoryItemSlots->AddChildToUniformGrid(ItemDataWidget, SlotIndex / RowsInGrid, SlotIndex % RowsInGrid);
        if (!GridObject) continue;
        GridObject->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
        GridObject->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
        ++SlotIndex;
    }
}
