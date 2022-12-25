// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/AwesomeEquipmentWidget.h"
#include "Player/AwesomeBaseCharacter.h"
#include "UI/AwesomeItemDataWidget.h"
#include "Components/SizeBox.h"

#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"

void UAwesomeEquipmentWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    InitEquipment();

    if (GetOwningPlayer())
    {
        GetOwningPlayer()->GetOnNewPawnNotifier().AddUObject(this, &UAwesomeEquipmentWidget::OnNewPawn);
        OnNewPawn(GetOwningPlayerPawn());
    }
}

void UAwesomeEquipmentWidget::InitEquipment()
{
    if (HeadSlotBox)
    {
        auto SlotWidget = CreateWidget<UAwesomeItemDataWidget>(GetOwningPlayer(), ItemDataWidgetClass);
        if (SlotWidget)
        {
            HeadSlotBox->AddChild(SlotWidget);
            EqiupmentSlotsMap.Add(EEquipmentType::Head, SlotWidget);
        }
    }
    if (RightArmSlotBox)
    {
        auto SlotWidget = CreateWidget<UAwesomeItemDataWidget>(GetOwningPlayer(), ItemDataWidgetClass);
        if (SlotWidget)
        {
            RightArmSlotBox->AddChild(SlotWidget);
            EqiupmentSlotsMap.Add(EEquipmentType::RightArm, SlotWidget);
        }
    }
    if (TorsoSlotBox)
    {
        auto SlotWidget = CreateWidget<UAwesomeItemDataWidget>(GetOwningPlayer(), ItemDataWidgetClass);
        if (SlotWidget)
        {
            TorsoSlotBox->AddChild(SlotWidget);
            EqiupmentSlotsMap.Add(EEquipmentType::Torso, SlotWidget);
        }
    }
    if (LeftArmSlotBox)
    {
        auto SlotWidget = CreateWidget<UAwesomeItemDataWidget>(GetOwningPlayer(), ItemDataWidgetClass);
        if (SlotWidget)
        {
            LeftArmSlotBox->AddChild(SlotWidget);
            EqiupmentSlotsMap.Add(EEquipmentType::LeftArm, SlotWidget);
        }
    }
    if (LegsSlotBox)
    {
        auto SlotWidget = CreateWidget<UAwesomeItemDataWidget>(GetOwningPlayer(), ItemDataWidgetClass);
        if (SlotWidget)
        {
            LegsSlotBox->AddChild(SlotWidget);
            EqiupmentSlotsMap.Add(EEquipmentType::Legs, SlotWidget);
        }
    }

    for (TPair<EEquipmentType, UAwesomeItemDataWidget*>& Element : EqiupmentSlotsMap)
    {
        Element.Value->SetSlotLocationType(ESlotLocationType::Equipment);
        Element.Value->SetEquipmentType(Element.Key);
    }
}

void UAwesomeEquipmentWidget::OnNewPawn(APawn* NewPawn)
{
    const auto Player = Cast<AAwesomeBaseCharacter>(NewPawn);
    if (!Player) return;
    if (!Player->OnEquipmentSlotDataChanged.IsBoundToObject(this))
    {
        Player->OnEquipmentSlotDataChanged.AddUObject(this, &UAwesomeEquipmentWidget::OnEquipmentSlotDataChanged);
    }
}

void UAwesomeEquipmentWidget::OnEquipmentSlotDataChanged(const FSlot& NewSlotData, EEquipmentType Type)
{
    auto SlotWidget = EqiupmentSlotsMap.FindChecked(Type);
    if (!SlotWidget) return;
    SlotWidget->SetDataSlot(NewSlotData);
}
