// Fill out your copyright notice in the Description page of Project Settings.

#include "Pickup/AwesomeBackpackMaster.h"
#include "Player/AwesomeBaseCharacter.h"

AAwesomeBackpackMaster::AAwesomeBackpackMaster()
{
    GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
    // FCollisionResponseContainer ResponseContainer;
    // ResponseContainer.SetResponse(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
    // ResponseContainer.SetResponse(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
    // GetStaticMeshComponent()->SetCollisionResponseToChannels(ResponseContainer);
}

void AAwesomeBackpackMaster::Interact(AActor* InteractiveActor)
{
    const auto Player = Cast<AAwesomeBaseCharacter>(InteractiveActor);
    if (!Player) return;

    Player->EquipBackpack(this);
}

bool AAwesomeBackpackMaster::FindStackOfSameItems(const FSlot& Item, uint8& OutSlotIndex, int32& OutAmount, bool& bOutCanStack)
{
    OutSlotIndex = -1;
    OutAmount = -1;
    bOutCanStack = false;
    if (!BackpackSlots.Num() || !Item.DataTableRowHandle.DataTable) return false;

    const auto ItemDataPointer = Item.DataTableRowHandle.DataTable->FindRow<FInventoryData>(Item.DataTableRowHandle.RowName, "", false);
    if (!ItemDataPointer) return false;
    const auto ItemData = *ItemDataPointer;
    bOutCanStack = ItemData.bCanStack;

    uint8 SlotIndex{0};
    for (const auto& SlotData : BackpackSlots)
    {
        if (!SlotData.DataTableRowHandle.DataTable) continue;
        const auto SlotItemDataPointer = SlotData.DataTableRowHandle.DataTable->FindRow<FInventoryData>(SlotData.DataTableRowHandle.RowName, "", false);
        if (!SlotItemDataPointer) continue;
        const auto SlotItemData = *SlotItemDataPointer;
        if (SlotItemData.Name == ItemData.Name)
        {
            OutSlotIndex = SlotIndex;
            OutAmount = SlotData.Amount;
            return true;
        }
        ++SlotIndex;
    }

    return false;
}

bool AAwesomeBackpackMaster::FindEmptySlot(uint8& OutSlotIndex)
{
    OutSlotIndex = -1;
    if (!BackpackSlots.Num()) return false;

    uint8 SlotIndex{0};
    for (const auto& SlotData : BackpackSlots)
    {
        if (!SlotData.Amount)
        {
            OutSlotIndex = SlotIndex;
            return true;
        }
        ++SlotIndex;
    }
    return false;
}

bool AAwesomeBackpackMaster::TryAddItemToSlots(const FSlot& Item)
{
    uint8 FoundSlotIndex;
    int32 FoundAmount;
    bool bCanStack;
    if (FindStackOfSameItems(Item, FoundSlotIndex, FoundAmount, bCanStack))
    {
        if (bCanStack)
        {
            return UpdateSlotItemData(FoundSlotIndex, Item.Amount);
        }

        if (!bCanStack && FindEmptySlot(FoundSlotIndex))
        {
            BackpackSlots[FoundSlotIndex] = Item;
            return UpdateSlotItemData(FoundSlotIndex, 0);
        }
        else
        {
            return false;
        }
    }
    else
    {
        if (FindEmptySlot(FoundSlotIndex))
        {
            BackpackSlots[FoundSlotIndex] = Item;
            return UpdateSlotItemData(FoundSlotIndex, 0);
        }
        return false;
    }
}

void AAwesomeBackpackMaster::BeginPlay()
{
    Super::BeginPlay();

    InitBackpack();
}

void AAwesomeBackpackMaster::InitBackpack()
{
    for (uint8 Index = 0; Index < BackpackSlotsNumber; ++Index)
    {
        BackpackSlots.Add(FSlot());
    }
}

bool AAwesomeBackpackMaster::UpdateSlotItemData(const uint8 Index, const int32 AmountModifier)
{
    if (!BackpackSlots.IsValidIndex(Index)) return false;
    const auto Result = BackpackSlots[Index].Amount + AmountModifier;
    BackpackSlots[Index].Amount = FMath::Clamp(Result, 0, 999);  // Set MAX in properties;
    OnSlotsChanged.Broadcast(BackpackSlots);
    return true;
}
