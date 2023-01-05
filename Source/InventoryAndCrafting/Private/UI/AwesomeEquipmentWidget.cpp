// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/AwesomeEquipmentWidget.h"
#include "UI/AwesomeItemDataWidget.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/InventoryComponent.h"

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

    auto BackSlotWidget = CreateWidget<UAwesomeItemDataWidget>(GetOwningPlayer(), ItemDataWidgetClass);
    InitEquipmentSlot(BackSlotBox, BackSlotWidget, EEquipmentType::Back);

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
    if (!NewPawn) return;
    const auto InventoryComponent = NewPawn->FindComponentByClass<UInventoryComponent>();
    if (!InventoryComponent) return;
    if (!InventoryComponent->OnEquipmentSlotDataChanged.IsBoundToObject(this))
    {
        InventoryComponent->OnEquipmentSlotDataChanged.AddUObject(this, &UAwesomeEquipmentWidget::OnEquipmentSlotDataChanged);
    }
    if (!InventoryComponent->OnMoneyValueChanged.IsBoundToObject(this))
    {
        InventoryComponent->OnMoneyValueChanged.AddUObject(this, &UAwesomeEquipmentWidget::OnMoneyValueChanged);
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

void UAwesomeEquipmentWidget::OnMoneyValueChanged(int32 Value)
{
    if (MoneyValueText)
    {
        UE_LOG(LogTemp, Display, TEXT("OnMoneyValueChanged"));
        MoneyValueText->SetText(FText::AsNumber(Value));
    }
}