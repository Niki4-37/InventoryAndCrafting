// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/ShopCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Components/InventoryComponent.h"

AShopCharacter::AShopCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
}

void AShopCharacter::StopTrading(AActor* Buyer)
{
    Buyers.Remove(Buyer);
}

void AShopCharacter::AddBuyer(AActor* Buyer)
{
    Buyers.Add(Buyer);
    UE_LOG(LogTemp, Display, TEXT("TotalBuyers: %i"), Buyers.Num());
}

void AShopCharacter::BuyItemInStore(uint8 Index)
{
    /* handled on server */
    if (ShopSlots.IsValidIndex(Index))
    {
        ShopSlots[Index].Amount -= 1;
        if (!ShopSlots[Index].Amount && ShopSlotsNumber < ShopSlots.Num())
        {
            ShopSlots.RemoveAt(Index);
        }
    }

    for (const auto& Buyer : Buyers)
    {
        if (!Buyer) continue;
        const auto InventoryComponent = Buyer->FindComponentByClass<UInventoryComponent>();
        if (!InventoryComponent) continue;
        InventoryComponent->UpdateShopWidgetAfterTransaction(ShopSlots);
    }
}

bool AShopCharacter::SellItemInStore(const FSlot& Item)
{
    /* handled on server */
    uint8 FoundSlotIndex;
    int32 FoundAmount;
    bool bCanStack;
    if (FindStackOfSameItems(Item, FoundSlotIndex, FoundAmount, bCanStack))
    {
        ShopSlots[FoundSlotIndex].Amount += FoundAmount;
    }

    if (FindEmptySlot(FoundSlotIndex))
    {
        ShopSlots[FoundSlotIndex] = Item;
    }
    else
    {
        ShopSlots.Add(Item);
    }

    for (const auto& Buyer : Buyers)
    {
        if (!Buyer) continue;
        const auto InventoryComponent = Buyer->FindComponentByClass<UInventoryComponent>();
        if (!InventoryComponent) continue;
        InventoryComponent->UpdateShopWidgetAfterTransaction(ShopSlots);
    }

    return true;
}

void AShopCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (GetRemoteRole() == ENetRole::ROLE_SimulatedProxy)
    {
        InitShop_OnServer();
    }
}

void AShopCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AShopCharacter, ShopSlots);
    DOREPLIFETIME(AShopCharacter, Buyers);
    DOREPLIFETIME(AShopCharacter, Money);
}

void AShopCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AShopCharacter::Interact(AActor* InteractiveActor)
{
    if (!InteractiveActor) return;
    const auto InventoryComponent = InteractiveActor->FindComponentByClass<UInventoryComponent>();
    if (!InventoryComponent) return;
    InventoryComponent->StartTrading_OnServer(this);
}

bool AShopCharacter::FindStackOfSameItems(const FSlot& Item, uint8& OutSlotIndex, int32& OutAmount, bool& bOutCanStack)
{
    /* handled on server */
    OutSlotIndex = -1;
    OutAmount = -1;
    bOutCanStack = false;
    if (!ShopSlots.Num()) return false;

    const auto ItemDataPointer = Item.DataTableRowHandle.GetRow<FItemData>("");
    if (!ItemDataPointer) return false;
    const auto ItemData = *ItemDataPointer;
    bOutCanStack = ItemData.bCanStack;
    if (!ItemData.bCanStack) return false;

    int32 SearchingElementIndex{-1};
    if (ShopSlots.Find(Item, SearchingElementIndex))
    {
        OutSlotIndex = SearchingElementIndex;
        OutAmount = ShopSlots[OutSlotIndex].Amount;
        return true;
    }

    return false;
}

bool AShopCharacter::FindEmptySlot(uint8& OutSlotIndex)
{
    /* handled on server */
    OutSlotIndex = -1;
    if (!ShopSlots.Num()) return false;

    uint8 SlotIndex{0};
    for (const auto& SlotData : ShopSlots)
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

void AShopCharacter::InitShop_OnServer_Implementation()
{
    uint8 GoodsNumber = Goods.Num();
    uint8 SlotsNeededNumber = ShopSlotsNumber - GoodsNumber;

    ShopSlots.Append(Goods);
    while (SlotsNeededNumber)
    {
        ShopSlots.Add(FSlot());
        --SlotsNeededNumber;
    }
}
