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
    auto HeadSlotWidget = CreateWidget<UAwesomeItemDataWidget>(GetOwningPlayer(), ItemDataWidgetClass);
    InitEquipmentSlot(HeadSlotBox, HeadSlotWidget, EEquipmentType::Head);

    auto RightArmSlotWidget = CreateWidget<UAwesomeItemDataWidget>(GetOwningPlayer(), ItemDataWidgetClass);
    InitEquipmentSlot(RightArmSlotBox, RightArmSlotWidget, EEquipmentType::RightArm);

    auto TorsoSlotWidget = CreateWidget<UAwesomeItemDataWidget>(GetOwningPlayer(), ItemDataWidgetClass);
    InitEquipmentSlot(TorsoSlotBox, TorsoSlotWidget, EEquipmentType::Torso);

    auto LeftArmSlotWidget = CreateWidget<UAwesomeItemDataWidget>(GetOwningPlayer(), ItemDataWidgetClass);
    InitEquipmentSlot(LeftArmSlotBox, LeftArmSlotWidget, EEquipmentType::LeftArm);

    auto LegsSlotWidget = CreateWidget<UAwesomeItemDataWidget>(GetOwningPlayer(), ItemDataWidgetClass);
    InitEquipmentSlot(LegsSlotBox, LegsSlotWidget, EEquipmentType::Legs);

    for (TPair<EEquipmentType, UAwesomeItemDataWidget*>& Element : EqiupmentSlotsMap)
    {
        Element.Value->SetSlotLocationType(ESlotLocationType::Equipment);
        Element.Value->SetEquipmentType(Element.Key);
    }
}

void UAwesomeEquipmentWidget::InitEquipmentSlot(USizeBox* Box, UAwesomeItemDataWidget* ItemWidget, EEquipmentType Type)
{
    if (!ItemWidget || !Box) return;

    if (DefaultEqiupmentIconsMap.Contains(Type))
    {
        ItemWidget->SetIconToWidget(DefaultEqiupmentIconsMap.FindRef(Type));
    }
    Box->AddChild(ItemWidget);
    EqiupmentSlotsMap.Add(Type, ItemWidget);
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
    if (!NewSlotData.Amount && DefaultEqiupmentIconsMap.Contains(Type))
    {
        SlotWidget->SetIconToWidget(DefaultEqiupmentIconsMap.FindRef(Type));
    }
}
