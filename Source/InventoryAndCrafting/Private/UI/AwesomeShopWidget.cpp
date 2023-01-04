// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/AwesomeShopWidget.h"
#include "UI/AwesomeItemDataWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Player/AwesomeBaseCharacter.h"
#include "UI/AwesomeDragDropItemOperation.h"

void UAwesomeShopWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (GetOwningPlayer())
    {
        GetOwningPlayer()->GetOnNewPawnNotifier().AddUObject(this, &UAwesomeShopWidget::OnNewPawn);
        OnNewPawn(GetOwningPlayerPawn());
    }
}

bool UAwesomeShopWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    auto DragDropOperation = Cast<UAwesomeDragDropItemOperation>(InOperation);
    if (!DragDropOperation) return true;

    const auto Player = Cast<AAwesomeBaseCharacter>(GetOwningPlayerPawn());
    if (!Player) return true;

    Player->MoveItem_OnServer(DragDropOperation->GetSlotData(),              //
                              DragDropOperation->GetItemFromLocationType(),  //
                              DragDropOperation->GetFromEquipmentType(),     //
                              DragDropOperation->GetFromSlotIndex(),         //
                              ESlotLocationType::ShopSlots,                  //
                              EEquipmentType::NotEquipment,                  //
                              0);

    return OnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UAwesomeShopWidget::OnNewPawn(APawn* NewPawn)
{
    const auto Player = Cast<AAwesomeBaseCharacter>(NewPawn);
    if (!Player) return;
    if (!Player->OnStuffEquiped.IsBoundToObject(this))
    {
        Player->OnStuffEquiped.AddUObject(this, &UAwesomeShopWidget::OnStuffEquiped);
    }
    if (!Player->OnSlotChanged.IsBoundToObject(this))
    {
        Player->OnSlotChanged.AddUObject(this, &UAwesomeShopWidget::OnSlotChanged);
    }
}

void UAwesomeShopWidget::OnStuffEquiped(const TArray<FSlot>& Slots, ESlotLocationType Type)
{
    if (!ShopItemSlots || Type != ESlotLocationType::ShopSlots) return;
    ShopItemSlots->ClearChildren();

    uint8 SlotIndex{0};
    for (const auto& SlotData : Slots)
    {
        auto ItemDataWidget = CreateWidget<UAwesomeItemDataWidget>(GetOwningPlayer(), ItemDataWidgetClass);
        if (!ItemDataWidget) continue;
        ItemDataWidget->SetDataSlot(SlotData);
        ItemDataWidget->SetItemIndex(SlotIndex);
        ItemDataWidget->SetSlotLocationType(ESlotLocationType::ShopSlots);
        auto GridObject = ShopItemSlots->AddChildToUniformGrid(ItemDataWidget, SlotIndex / SlotsInRow, SlotIndex % SlotsInRow);
        if (!GridObject) continue;
        GridObject->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
        GridObject->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
        ++SlotIndex;
    }
}

void UAwesomeShopWidget::OnSlotChanged(const FSlot& NewSlotData, const uint8 SlotIndex, ESlotLocationType Type)
{
    if (!ShopItemSlots || Type != ESlotLocationType::ShopSlots) return;
    auto ItemDataWidget = Cast<UAwesomeItemDataWidget>(ShopItemSlots->GetChildAt(SlotIndex));
    if (!ItemDataWidget) return;
    ItemDataWidget->SetDataSlot(NewSlotData);
}
