// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/EquipmentWidget.h"
#include "UI/ItemDataWidget.h"
#include "UI/PreviewPlayer.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/InventoryComponent.h"

void UEquipmentWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    InitEquipment();

    if (GetOwningPlayer())
    {
        GetOwningPlayer()->GetOnNewPawnNotifier().AddUObject(this, &UEquipmentWidget::OnNewPawn);
        OnNewPawn(GetOwningPlayerPawn());
    }
}

void UEquipmentWidget::InitEquipment()
{
    auto HeadSlotWidget = CreateWidget<UItemDataWidget>(GetOwningPlayer(), ItemDataWidgetClass);
    InitEquipmentSlot(HeadSlotBox, HeadSlotWidget, EEquipmentType::Head);

    auto BackSlotWidget = CreateWidget<UItemDataWidget>(GetOwningPlayer(), ItemDataWidgetClass);
    InitEquipmentSlot(BackSlotBox, BackSlotWidget, EEquipmentType::Back);

    auto RightArmSlotWidget = CreateWidget<UItemDataWidget>(GetOwningPlayer(), ItemDataWidgetClass);
    InitEquipmentSlot(RightArmSlotBox, RightArmSlotWidget, EEquipmentType::RightArm);

    auto TorsoSlotWidget = CreateWidget<UItemDataWidget>(GetOwningPlayer(), ItemDataWidgetClass);
    InitEquipmentSlot(TorsoSlotBox, TorsoSlotWidget, EEquipmentType::Torso);

    auto LeftArmSlotWidget = CreateWidget<UItemDataWidget>(GetOwningPlayer(), ItemDataWidgetClass);
    InitEquipmentSlot(LeftArmSlotBox, LeftArmSlotWidget, EEquipmentType::LeftArm);

    auto LegsSlotWidget = CreateWidget<UItemDataWidget>(GetOwningPlayer(), ItemDataWidgetClass);
    InitEquipmentSlot(LegsSlotBox, LegsSlotWidget, EEquipmentType::Legs);

    for (TPair<EEquipmentType, UItemDataWidget*>& Element : EqiupmentSlotsMap)
    {
        Element.Value->SetSlotLocationType(ESlotLocationType::Equipment);
        Element.Value->SetEquipmentType(Element.Key);
    }
}

void UEquipmentWidget::InitEquipmentSlot(USizeBox* Box, UItemDataWidget* ItemWidget, EEquipmentType Type)
{
    if (!ItemWidget || !Box) return;

    if (DefaultEqiupmentIconsMap.Contains(Type))
    {
        ItemWidget->SetDisplayingInfo(DefaultEqiupmentIconsMap.FindRef(Type));
    }
    Box->AddChild(ItemWidget);
    EqiupmentSlotsMap.Add(Type, ItemWidget);
}

void UEquipmentWidget::OnNewPawn(APawn* NewPawn)
{
    if (!NewPawn) return;
    const auto InventoryComponent = NewPawn->FindComponentByClass<UInventoryComponent>();
    if (!InventoryComponent) return;
    if (!InventoryComponent->OnEquipmentSlotDataChanged.IsBoundToObject(this))
    {
        InventoryComponent->OnEquipmentSlotDataChanged.AddUObject(this, &UEquipmentWidget::OnEquipmentSlotDataChanged);
    }
    if (!InventoryComponent->OnMoneyValueChanged.IsBoundToObject(this))
    {
        InventoryComponent->OnMoneyValueChanged.AddUObject(this, &UEquipmentWidget::OnMoneyValueChanged);
    }

    if (PreviewPlayer)
    {
        PreviewPlayer->Destroy();
    }
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    SpawnParams.Owner = NewPawn;
    PreviewPlayer = GetWorld()->SpawnActor<APreviewPlayer>(PreviewPlayerClass, PreviewPlayerSpawnLocation, FRotator::ZeroRotator, SpawnParams);
    if (PreviewPlayer)
    {
        const auto SkeletalMeshComponent = NewPawn->FindComponentByClass<USkeletalMeshComponent>();
        if (SkeletalMeshComponent && SkeletalMeshComponent->SkeletalMesh && SkeletalMeshComponent->GetAnimClass())
        {
            const auto Mesh = SkeletalMeshComponent->SkeletalMesh;
            const auto AnimClass = SkeletalMeshComponent->GetAnimClass();
            PreviewPlayer->SetSkeletalMeshAndAnimation(Mesh, AnimClass);
        }
        PreviewPlayer->CreateComponents();
        RenderTargetMaterial = PreviewPlayer->CreateDynamicMaterialInstance();
    }
}

void UEquipmentWidget::OnEquipmentSlotDataChanged(const FSlot& NewSlotData, EEquipmentType Type, UStaticMesh* NewMesh)
{
    auto SlotWidget = EqiupmentSlotsMap.FindChecked(Type);
    if (!SlotWidget) return;
    SlotWidget->SetDataSlot(NewSlotData);
    if (!NewSlotData.Amount && DefaultEqiupmentIconsMap.Contains(Type))
    {
        SlotWidget->SetDisplayingInfo(DefaultEqiupmentIconsMap.FindRef(Type));
    }
    if (PreviewPlayer)
    {
        PreviewPlayer->SetEquipmentMesh(Type, NewMesh);
    }
}

void UEquipmentWidget::OnMoneyValueChanged(int32 Value)
{
    if (MoneyValueText)
    {
        UE_LOG(LogTemp, Display, TEXT("OnMoneyValueChanged"));
        MoneyValueText->SetText(FText::AsNumber(Value));
    }
}