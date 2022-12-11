// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/AwesomeEquipmentWidget.h"
#include "Player/AwesomeBaseCharacter.h"
#include "UI/AwesomeItemDataWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"

void UAwesomeEquipmentWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (GetOwningPlayer())
    {
        GetOwningPlayer()->GetOnNewPawnNotifier().AddUObject(this, &UAwesomeEquipmentWidget::OnNewPawn);
        OnNewPawn(GetOwningPlayerPawn());
    }
}

void UAwesomeEquipmentWidget::OnNewPawn(APawn* NewPawn)
{
    const auto Player = Cast<AAwesomeBaseCharacter>(NewPawn);
    if (!Player) return;

    // Player->OnItemPickedup.AddUObject(this, &UAwesomeEquipmentWidget::OnItemPickedup);

    UpdateItemSlots(Player->GetSlots());
}

void UAwesomeEquipmentWidget::UpdateItemSlots(const TArray<FSlot>& Slots)
{
    if (!EquipmentItemSlots) return;
    EquipmentItemSlots->ClearChildren();

    uint8 SlotIndex{0};
    for (const auto& SlotData : Slots)
    {
        auto ItemDataWidget = CreateWidget<UAwesomeItemDataWidget>(GetOwningPlayer(), ItemDataWidgetClass);
        if (!ItemDataWidget) continue;
        ItemDataWidget->SetDataFromItem(SlotData);
        auto GridObject = EquipmentItemSlots->AddChildToUniformGrid(ItemDataWidget, SlotIndex / SlotsInRow, SlotIndex % SlotsInRow);
        if (!GridObject) continue;
        GridObject->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
        GridObject->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
        ++SlotIndex;
    }
}
