// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/AwesomePersonalSlotsWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Player/AwesomeBaseCharacter.h"
#include "UI/AwesomeItemDataWidget.h"

void UAwesomePersonalSlotsWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (GetOwningPlayer())
    {
        GetOwningPlayer()->GetOnNewPawnNotifier().AddUObject(this, &UAwesomePersonalSlotsWidget::OnNewPawn);
        OnNewPawn(GetOwningPlayerPawn());
    }
}

void UAwesomePersonalSlotsWidget::OnNewPawn(APawn* NewPawn)
{
    const auto Player = Cast<AAwesomeBaseCharacter>(NewPawn);
    if (!Player) return;

    Player->OnSlotChanged.AddUObject(this, &UAwesomePersonalSlotsWidget::UpdateItemSlot);
    Player->OnStuffEquiped.AddUObject(this, &UAwesomePersonalSlotsWidget::OnStuffEquiped);
}

void UAwesomePersonalSlotsWidget::OnStuffEquiped(const TArray<FSlot>& Slots, ESlotLocationType Type)
{
    if (!PersonalSlots || Type != ESlotLocationType::PersonalSlots) return;
    PersonalSlots->ClearChildren();

    uint8 SlotIndex{0};
    for (const auto& SlotData : Slots)
    {
        auto ItemDataWidget = CreateWidget<UAwesomeItemDataWidget>(GetOwningPlayer(), ItemDataWidgetClass);
        if (!ItemDataWidget) continue;
        ItemDataWidget->SetDataSlot(SlotData);
        ItemDataWidget->SetItemIndex(SlotIndex);
        ItemDataWidget->SetSlotLocationType(ESlotLocationType::PersonalSlots);
        auto GridObject = PersonalSlots->AddChildToUniformGrid(ItemDataWidget, SlotIndex / SlotsInRow, SlotIndex % SlotsInRow);
        if (!GridObject) continue;
        GridObject->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
        GridObject->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
        ++SlotIndex;
    }
}

void UAwesomePersonalSlotsWidget::UpdateItemSlot(const FSlot& NewSlotData, const uint8 SlotIndex)
{
    if (!PersonalSlots) return;
    auto ItemDataWidget = Cast<UAwesomeItemDataWidget>(PersonalSlots->GetChildAt(SlotIndex));
    if (!ItemDataWidget) return;
    ItemDataWidget->SetDataSlot(NewSlotData);
}
