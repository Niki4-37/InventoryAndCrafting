// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/AwesomeInventoryWidget.h"
#include "Player/AwesomeBaseCharacter.h"
#include "UI/AwesomeItemDataWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Pickup/AwesomeBackpackMaster.h"

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
    if (!Player->OnStuffEquiped.IsBoundToObject(this))
    {
        Player->OnStuffEquiped.AddUObject(this, &UAwesomeInventoryWidget::OnStuffEquiped);
    }

    if (!Player->GetBackpack()) return;
    if (!Player->GetBackpack()->OnSlotsChanged.IsBoundToObject(this))
    {
        Player->GetBackpack()->OnSlotsChanged.AddUObject(this, &UAwesomeInventoryWidget::OnSlotsChanged);
    }

    UpdateItemSlots(Player->GetBackpackSlots());
}

void UAwesomeInventoryWidget::OnStuffEquiped()
{
    const auto Player = Cast<AAwesomeBaseCharacter>(GetOwningPlayerPawn());
    if (!Player || !Player->GetBackpack()) return;
    if (!Player->GetBackpack()->OnSlotsChanged.IsBoundToObject(this))
    {
        Player->GetBackpack()->OnSlotsChanged.AddUObject(this, &UAwesomeInventoryWidget::OnSlotsChanged);
    }
    UpdateItemSlots(Player->GetBackpackSlots());
}

void UAwesomeInventoryWidget::OnSlotsChanged(const TArray<FSlot>& Slots)
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
        auto GridObject = InventoryItemSlots->AddChildToUniformGrid(ItemDataWidget, SlotIndex / SlotsInRow, SlotIndex % SlotsInRow);
        if (!GridObject) continue;
        GridObject->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
        GridObject->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
        ++SlotIndex;
    }
}
