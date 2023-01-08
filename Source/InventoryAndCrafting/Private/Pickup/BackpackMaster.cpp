// Fill out your copyright notice in the Description page of Project Settings.

#include "Pickup/BackpackMaster.h"
#include "Net/UnrealNetwork.h"

#include "Components/InventoryComponent.h"

ABackpackMaster::ABackpackMaster()
{
    bReplicates = true;
    SetReplicatingMovement(true);
    GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
    FCollisionResponseContainer ResponseContainer;
    ResponseContainer.SetResponse(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
    ResponseContainer.SetResponse(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
    GetStaticMeshComponent()->SetCollisionResponseToChannels(ResponseContainer);
}

void ABackpackMaster::Interact(AActor* InteractiveActor)
{
    if (!InteractiveActor) return;
    const auto InventoryComponent = InteractiveActor->FindComponentByClass<UInventoryComponent>();
    if (!InventoryComponent) return;

    InventoryComponent->EquipBackpack_OnServer(this);
}

bool ABackpackMaster::FindStackOfSameItems(const FSlot& Item, uint8& OutSlotIndex, int32& OutAmount, bool& bOutCanStack)
{
    OutSlotIndex = -1;
    OutAmount = -1;
    bOutCanStack = false;
    if (!BackpackSlots.Num() || !Item.DataTableRowHandle.DataTable) return false;

    const auto ItemDataPointer = Item.DataTableRowHandle.DataTable->FindRow<FItemData>(Item.DataTableRowHandle.RowName, "", false);
    if (!ItemDataPointer) return false;
    const auto ItemData = *ItemDataPointer;
    bOutCanStack = ItemData.bCanStack;

    int32 SearchingElementIndex{-1};
    if (BackpackSlots.Find(Item, SearchingElementIndex))
    {
        OutSlotIndex = SearchingElementIndex;
        OutAmount = BackpackSlots[OutSlotIndex].Amount;
        return true;
    }

    return false;
}

bool ABackpackMaster::FindEmptySlot(uint8& OutSlotIndex)
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

bool ABackpackMaster::TryAddItemToSlots(const FSlot& Item)
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

bool ABackpackMaster::RemoveAmountFromInventorySlotsAtIndex(const uint8 Index, const int32 AmountToRemove)
{
    return UpdateSlotItemData(Index, -AmountToRemove);
}

void ABackpackMaster::BeginPlay()
{
    Super::BeginPlay();

    // if (GetRemoteRole() == ENetRole::ROLE_SimulatedProxy)
    //{
    //     InitBackpack_OnServer();
    // }
}

void ABackpackMaster::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ABackpackMaster, BackpackSlots);
    DOREPLIFETIME(ABackpackMaster, BackpackSlotsNumber);
}

void ABackpackMaster::InitBackpack_OnServer_Implementation()
{
    for (uint8 Index = 0; Index < BackpackSlotsNumber; ++Index)
    {
        BackpackSlots.Add(FSlot());
    }
}

bool ABackpackMaster::UpdateSlotItemData(const uint8 Index, const int32 AmountModifier)
{
    if (!BackpackSlots.IsValidIndex(Index)) return false;
    const auto Result = BackpackSlots[Index].Amount + AmountModifier;
    BackpackSlots[Index].Amount = FMath::Clamp(Result, 0, 99);  // Set MAX in properties;
    if (!BackpackSlots[Index].Amount)
    {
        BackpackSlots[Index] = FSlot();
    }
    UpdateOwnerWidget(BackpackSlots[Index], Index);
    UE_LOG(LogTemp, Display, TEXT("%s added to slot %i"), *BackpackSlots[Index].DataTableRowHandle.RowName.ToString(), Index);
    return true;
}

void ABackpackMaster::UpdateOwnerWidget(const FSlot& Item, const uint8 Index)
{
    if (!GetOwner()) return;
    const auto InventoryComponent = GetOwner()->FindComponentByClass<UInventoryComponent>();
    if (!InventoryComponent) return;

    InventoryComponent->UpdateWidgetSlotData(Item, Index, ESlotLocationType::Inventory);
}