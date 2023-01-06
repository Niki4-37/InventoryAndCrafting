// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Pickup/AwesomeBackpackMaster.h"
#include "Pickup/AwesomePickupMaster.h"
#include "AI/AwesomeShop.h"
#include "GameFramework/Character.h"
#include "Equipment/AwesomeEquipmentActor.h"

#include "Player/AwesomePlayerController.h"

DEFINE_LOG_CATEGORY_STATIC(InventoryComponent_LOG, All, All);

UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);

    for (EEquipmentType EquipmentType = EEquipmentType::Begin; EquipmentType != EEquipmentType::End; ++EquipmentType)
    {
        FString EnumNameString(UEnum::GetValueAsName(EquipmentType).ToString());
        int32 ScopeIndex = EnumNameString.Find(TEXT("::"), ESearchCase::CaseSensitive);
        if (ScopeIndex != INDEX_NONE)
        {
            FName SocketName = FName(*(EnumNameString.Mid(ScopeIndex + 2) + "Socket"));
            EquipmentSocketNamesMap.Add(EquipmentType, SocketName);
        }
    }
}

void UInventoryComponent::EquipBackpack_OnServer_Implementation(AAwesomeBackpackMaster* Backpack)
{
    const auto Character = Cast<ACharacter>(GetOwner());
    if (!Backpack || !Character) return;

    if (EquipedBackpack)
    {
        FDetachmentTransformRules DetachmentRules(EDetachmentRule::KeepWorld, false);
        EquipedBackpack->DetachFromActor(DetachmentRules);
        EquipedBackpack->GetStaticMeshComponent()->SetSimulatePhysics(true);
    }

    EquipedBackpack = Backpack;
    EquipedBackpack->GetStaticMeshComponent()->SetSimulatePhysics(false);
    FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
    EquipedBackpack->AttachToComponent(Character->GetMesh(), AttachmentRules, BackpackSocketName);

    EquipedBackpack->SetOwner(GetOwner());

    OnStuffEquiped_OnClient(EquipedBackpack->GetBackpackSlots(), ESlotLocationType::Inventory);
}

void UInventoryComponent::StartTrading_OnServer_Implementation(AAwesomeShop* Shop)
{
    if (!Shop || ActiveShop) return;
    ActiveShop = Shop;
    ActiveShop->AddBuyer(GetOwner());
    OnStuffEquiped_OnClient(Shop->GetGoods(), ESlotLocationType::ShopSlots);
    OnTrading_OnClient(true);
    OpenInventory_OnClient();
}

void UInventoryComponent::StopTrading_OnServer_Implementation()
{
    if (!ActiveShop) return;
    ActiveShop->StopTrading(GetOwner());
    ActiveShop = nullptr;
}

void UInventoryComponent::PickupItem_OnServer_Implementation(AAwesomePickupMaster* Pickup)
{
    if (!Pickup) return;
    if (TryAddItemToSlots(Pickup->GetPickupItem()))
    {
        Pickup->Destroy();
    }
}

void UInventoryComponent::MoveItem_OnServer_Implementation(const FSlot& Item,                   //
                                                           ESlotLocationType FromLocationType,  //
                                                           EEquipmentType FromEquipmentType,    //
                                                           const uint8 FromSlotIndex,           //
                                                           ESlotLocationType ToLocationType,    //
                                                           EEquipmentType ToEquipmentType,      //
                                                           const uint8 ToSlotIndex)
{
    bool bMoveSuccess{false};
    switch (ToLocationType)
    {
        case (ESlotLocationType::Inventory):
        {
            bMoveSuccess = EquipedBackpack ? EquipedBackpack->TryAddItemToSlots(Item) : false;
            break;
        }
        case (ESlotLocationType::PersonalSlots): bMoveSuccess = TryAddItemToPersonalSlotsByIndex(Item, ToSlotIndex); break;
        case (ESlotLocationType::Equipment): bMoveSuccess = TryAddItemToEquipment(Item, ToEquipmentType); break;
        case (ESlotLocationType::Environment): bMoveSuccess = DropItem(Item); break;
        case (ESlotLocationType::ShopSlots): bMoveSuccess = TrySellItem(Item); break;

        default: bMoveSuccess = false;
    }

    if (!bMoveSuccess) return;

    switch (FromLocationType)
    {
        case (ESlotLocationType::Inventory):
        {
            if (!EquipedBackpack) return;
            GetBackpack()->RemoveAmountFromInventorySlotsAtIndex(FromSlotIndex, Item.Amount);
            break;
        }
        case (ESlotLocationType::PersonalSlots): RemoveAmountFromChoosenSlotsAtIndex(PersonalSlots, FromSlotIndex, Item.Amount); break;
        case (ESlotLocationType::Equipment): RemoveItemFromEquipment(FromEquipmentType); break;
        case (ESlotLocationType::ShopSlots): BuyItem(Item, FromSlotIndex);
    }
}

void UInventoryComponent::UpdateWidgetSlotData(const FSlot& Item, const uint8 Index, ESlotLocationType Type)
{
    OnSlotChanged_OnClient(Item, Index, Type);
}

void UInventoryComponent::UpdateShopWidgetAfterTransaction(const TArray<FSlot>& Goods)
{
    OnStuffEquiped_OnClient(Goods, ESlotLocationType::ShopSlots);
}

bool UInventoryComponent::HasEquipmentToSwap(EEquipmentType FirstSlotType, EEquipmentType SecondSlotType)
{
    const auto FirstSlotPtr = EquipmentSlots.FindByPredicate([&](FEquipmentSlot& Data) { return Data.EquipmentType == FirstSlotType; });
    const auto SecondSlotPtr = EquipmentSlots.FindByPredicate([&](FEquipmentSlot& Data) { return Data.EquipmentType == SecondSlotType; });

    if (!FirstSlotPtr || !SecondSlotPtr) return false;

    return FirstSlotPtr->Amount || SecondSlotPtr->Amount;
}

void UInventoryComponent::SwapItems(EEquipmentType FirstSlotType, EEquipmentType SecondSlotType)
{
    const auto ItemFromFirstSlot = RemoveItemFromEquipment(FirstSlotType);
    const auto ItemFromSecondSlot = RemoveItemFromEquipment(SecondSlotType);

    TryAddItemToEquipment(ItemFromFirstSlot, SecondSlotType);
    TryAddItemToEquipment(ItemFromSecondSlot, FirstSlotType);
}

bool UInventoryComponent::DropItem(const FSlot& Item)
{
    /* handled on server */
    if (!GetWorld() || !Item.Amount) return false;
    const auto SpawningLocation = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 200.f;
    FTransform SpawnTransform(FRotator::ZeroRotator, SpawningLocation, FVector(1.f));

    auto Pickup = GetWorld()->SpawnActorDeferred<AAwesomePickupMaster>(PickupMasterClass, SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
    if (Pickup)
    {
        Pickup->SetPickupItem(Item);
        Pickup->FinishSpawning(SpawnTransform);
        UE_LOG(InventoryComponent_LOG, Display, TEXT("%s spawn in %s"), *Pickup->GetName(), *SpawningLocation.ToString());
        return true;
    }

    return false;
}

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    if (GetOwner()->HasAuthority())
    {
        Money = 100;
        InitEnableSlots_OnServer();
    }
    OnMoneyChanged_OnClient(Money);
    OnStuffEquiped.Broadcast(PersonalSlots, ESlotLocationType::PersonalSlots);
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UInventoryComponent, PersonalSlotsNumber);
    DOREPLIFETIME(UInventoryComponent, Money);
    DOREPLIFETIME(UInventoryComponent, PersonalSlots);
    DOREPLIFETIME(UInventoryComponent, EquipmentSlots);
    DOREPLIFETIME(UInventoryComponent, EquipedBackpack);
    DOREPLIFETIME(UInventoryComponent, ActiveShop);
}

void UInventoryComponent::InitEnableSlots_OnServer_Implementation()
{
    for (uint8 Index = 0; Index < PersonalSlotsNumber; ++Index)
    {
        PersonalSlots.Add(FSlot());
    }

    for (EEquipmentType EquipmentType = EEquipmentType::Begin; EquipmentType != EEquipmentType::End; ++EquipmentType)
    {
        EquipmentSlots.Add(FEquipmentSlot(EquipmentType, FDataTableRowHandle(), 0));
        UE_LOG(InventoryComponent_LOG, Display, TEXT("%s"), *UEnum::GetValueAsString(EquipmentType));
    }
}

bool UInventoryComponent::FindStackOfSameItems(const FSlot& Item, uint8& OutSlotIndex, int32& OutAmount, bool& bOutCanStack)
{
    /* handled on server */
    OutSlotIndex = -1;
    OutAmount = -1;
    bOutCanStack = false;
    if (!PersonalSlots.Num()) return false;

    const auto ItemDataPointer = Item.DataTableRowHandle.GetRow<FItemData>("");
    if (!ItemDataPointer) return false;
    const auto ItemData = *ItemDataPointer;
    bOutCanStack = ItemData.bCanStack;
    if (!ItemData.bCanStack) return false;

    int32 SearchingElementIndex{-1};
    if (PersonalSlots.Find(Item, SearchingElementIndex))
    {
        OutSlotIndex = SearchingElementIndex;
        OutAmount = PersonalSlots[OutSlotIndex].Amount;
        return true;
    }

    return false;
}

bool UInventoryComponent::FindEmptySlot(uint8& OutSlotIndex)
{
    /* handled on server */
    OutSlotIndex = -1;
    if (!PersonalSlots.Num()) return false;

    uint8 SlotIndex{0};
    for (const auto& SlotData : PersonalSlots)
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

bool UInventoryComponent::TryAddItemToSlots(const FSlot& Item)
{
    /* handled on server */
    uint8 FoundSlotIndex;
    int32 FoundAmount;
    bool bCanStack;

    if (EquipedBackpack && EquipedBackpack->TryAddItemToSlots(Item))
    {
        return true;
    }

    if (FindStackOfSameItems(Item, FoundSlotIndex, FoundAmount, bCanStack))
    {
        if (bCanStack)
        {
            return UpdateSlotItemData(PersonalSlots, FoundSlotIndex, Item.Amount);
        }

        if (!bCanStack && FindEmptySlot(FoundSlotIndex))
        {
            PersonalSlots[FoundSlotIndex] = Item;
            return UpdateSlotItemData(PersonalSlots, FoundSlotIndex, 0);
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
            PersonalSlots[FoundSlotIndex] = Item;
            return UpdateSlotItemData(PersonalSlots, FoundSlotIndex, 0);
        }
        return false;
    }
}

bool UInventoryComponent::TryAddItemToPersonalSlotsByIndex(const FSlot& Item, const uint8 InIndex)
{
    /* handled on server */
    if (!PersonalSlots.IsValidIndex(InIndex)) return false;

    if (!PersonalSlots[InIndex].Amount)
    {
        PersonalSlots[InIndex] = Item;
        return UpdateSlotItemData(PersonalSlots, InIndex, 0);
    }

    if (Item.DataTableRowHandle.RowName == PersonalSlots[InIndex].DataTableRowHandle.RowName)
    {
        const auto ItemDataPointer = Item.DataTableRowHandle.GetRow<FItemData>("");
        if (!ItemDataPointer) return false;
        const auto ItemData = *ItemDataPointer;
        if (!ItemData.bCanStack) return false;

        return UpdateSlotItemData(PersonalSlots, InIndex, Item.Amount);
    }
    return false;
}

bool UInventoryComponent::TryAddItemToEquipment(const FSlot& Item, EEquipmentType ToEquipmentType)
{
    /* handled on server */
    if (!Item.Amount) return false;
    const auto ItemDataPointer = Item.DataTableRowHandle.GetRow<FItemData>("");
    if (!ItemDataPointer) return false;
    const auto ItemData = *ItemDataPointer;
    auto ItemEquipmentType = ItemData.EquipmnetType;
    if (ItemData.EquipmnetType == EEquipmentType::NotEquipment) return false;

    CheckForWeapon(ItemEquipmentType, ToEquipmentType);

    auto FoundSlotPtr = EquipmentSlots.FindByPredicate([&](FEquipmentSlot& Data) { return Data.EquipmentType == ToEquipmentType; });
    if (!FoundSlotPtr) return false;

    auto FoundSlot = *FoundSlotPtr;

    if (FoundSlot.Amount)
    {
        UE_LOG(InventoryComponent_LOG, Display, TEXT("Slot occupied"));
        if (TryAddItemToSlots(FSlot(FoundSlot.DataTableRowHandle, FoundSlot.Amount)))
        {
            RemoveItemFromEquipment(ItemEquipmentType);
        }
        else
            return false;
    }
    *FoundSlotPtr = FEquipmentSlot(ItemEquipmentType, Item.DataTableRowHandle, Item.Amount);

    if (ItemData.bCanIncreasePersonalSlots)
    {
        PersonalSlots.Append(ItemData.PresonalExtraSlots);
        OnStuffEquiped_OnClient(PersonalSlots, ESlotLocationType::PersonalSlots);
    }

    OnEquipmentSlotDataChanged_OnClient(Item, ItemEquipmentType);
    const auto SocketName = EquipmentSocketNamesMap.FindRef(ItemEquipmentType);
    EquipItem(ItemData.ActorClass, ItemData.Mesh, SocketName, ItemEquipmentType);

    return true;
}

bool UInventoryComponent::CheckForWeapon(EEquipmentType& WeaponEquipmentType, EEquipmentType ToEquipmentType)
{
    switch (ToEquipmentType)
    {
        case (EEquipmentType::RightArm):
        {
            if (WeaponEquipmentType == EEquipmentType::RightArm) return true;
        }
        case (EEquipmentType::LeftArm):
        {
            if (WeaponEquipmentType == EEquipmentType::RightArm)
            {
                WeaponEquipmentType = ToEquipmentType;
                return true;
            }
        }
        case (EEquipmentType::Back):
        {
            if (WeaponEquipmentType == EEquipmentType::RightArm)
            {
                WeaponEquipmentType = ToEquipmentType;
                return true;
            }
            if (WeaponEquipmentType == EEquipmentType::LeftArm)
            {
                WeaponEquipmentType = ToEquipmentType;
                return true;
            }
        }
        default: return false;
    }
}

bool UInventoryComponent::RemoveAmountFromChoosenSlotsAtIndex(TArray<FSlot>& Slots, const uint8 Index, const int32 AmountToRemove)
{
    /* handled on server */
    return UpdateSlotItemData(Slots, Index, -AmountToRemove);
}

bool UInventoryComponent::RemoveItemFromPersonalSlots(const FSlot& Item)
{
    /* handled on server */
    uint8 FoundSlotIndex;
    int32 FoundAmount;
    bool bCanStack;
    if (FindStackOfSameItems(Item, FoundSlotIndex, FoundAmount, bCanStack))
    {
        if (RemoveAmountFromChoosenSlotsAtIndex(PersonalSlots, FoundSlotIndex, --FoundAmount)) return true;
    }
    return false;
}

FSlot UInventoryComponent::RemoveItemFromEquipment(EEquipmentType FromEquipmentType)
{
    auto FoundSlotPtr = EquipmentSlots.FindByPredicate([&](FEquipmentSlot& Data) { return Data.EquipmentType == FromEquipmentType; });
    if (!FoundSlotPtr || !FoundSlotPtr->Amount) return FSlot();
    auto FoundSlot = *FoundSlotPtr;
    *FoundSlotPtr = FEquipmentSlot(FromEquipmentType, FDataTableRowHandle(), 0);

    if (auto FoundRow = FoundSlot.DataTableRowHandle.GetRow<FItemData>(""))
    {
        RemovePersonalExtraSlots(*FoundRow);
    }

    OnEquipmentSlotDataChanged_OnClient(FSlot(), FromEquipmentType);
    if (auto EquippedItem = EquippedItemsMap.FindAndRemoveChecked(FromEquipmentType))
    {
        EquippedItem->Destroy();
    }

    return FSlot(FoundSlot.DataTableRowHandle, FoundSlot.Amount);
}

void UInventoryComponent::RemovePersonalExtraSlots(const FItemData& ItemData)
{
    if (!ItemData.bCanIncreasePersonalSlots) return;
    auto ExtraSlotsNumber = ItemData.PresonalExtraSlots.Num();
    while (ExtraSlotsNumber)
    {
        DropItem(PersonalSlots.Last());
        PersonalSlots.Pop();
        --ExtraSlotsNumber;
    }
    OnStuffEquiped_OnClient(PersonalSlots, ESlotLocationType::PersonalSlots);
}

bool UInventoryComponent::UpdateSlotItemData(TArray<FSlot>& Slots, const uint8 Index, const int32 AmountModifier)
{
    /* handled on server */
    if (!Slots.IsValidIndex(Index)) return false;
    const auto Result = Slots[Index].Amount + AmountModifier;
    if (Result > 99) return false;
    Slots[Index].Amount = FMath::Clamp(Result, 0, 99);  // Set MAX in properties;
    if (!Slots[Index].Amount)
    {
        Slots[Index] = FSlot();
    }

    OnSlotChanged_OnClient(Slots[Index], Index, ESlotLocationType::PersonalSlots);
    return true;
}

void UInventoryComponent::EquipItem(UClass* Class, UStaticMesh* NewMesh, FName SocketName, EEquipmentType Type)
{
    /* handled on server */
    const auto Character = Cast<ACharacter>(GetOwner());
    if (!GetWorld() || !Character) return;
    auto EquippedItem = GetWorld()->SpawnActor<AAwesomeEquipmentActor>(Class);
    if (!EquippedItem) return;
    SetStaticMesh_Multicast(EquippedItem, NewMesh);
    FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
    EquippedItem->AttachToComponent(Character->GetMesh(), AttachmentRules, SocketName);
    EquippedItemsMap.Add(Type, EquippedItem);
}

bool UInventoryComponent::TrySellItem(const FSlot& SellingItem)
{
    /* handled on server */
    if (!ActiveShop) return false;

    const auto ItemDataPtr = SellingItem.DataTableRowHandle.GetRow<FItemData>("");
    if (!ItemDataPtr) return false;
    Money += ItemDataPtr->Cost;

    OnMoneyChanged_OnClient(Money);

    return ActiveShop->SellItem(SellingItem);
}

void UInventoryComponent::BuyItem(const FSlot& BuyingItem, uint8 Index)
{
    /* handle on server */
    if (!ActiveShop) return;

    const auto ItemDataPtr = BuyingItem.DataTableRowHandle.GetRow<FItemData>("");
    if (!ItemDataPtr) return;
    if (ItemDataPtr->Cost > Money) return;
    Money -= ItemDataPtr->Cost;

    OnMoneyChanged_OnClient(Money);

    ActiveShop->BuyItem(Index);
}

void UInventoryComponent::OnStuffEquiped_OnClient_Implementation(const TArray<FSlot>& Slots, ESlotLocationType Type)
{
    OnStuffEquiped.Broadcast(Slots, Type);
}

void UInventoryComponent::OnSlotChanged_OnClient_Implementation(const FSlot& Item, const uint8 Index, ESlotLocationType Type)
{
    OnSlotChanged.Broadcast(Item, Index, Type);
}

void UInventoryComponent::OnEquipmentSlotDataChanged_OnClient_Implementation(const FSlot& Item, EEquipmentType Type)
{
    OnEquipmentSlotDataChanged.Broadcast(Item, Type);
}

void UInventoryComponent::SetStaticMesh_Multicast_Implementation(AAwesomeEquipmentActor* Actor, UStaticMesh* NewMesh)
{

    if (!Actor || !NewMesh) return;
    Actor->SetStaticMesh(NewMesh);
}

void UInventoryComponent::OnTrading_OnClient_Implementation(bool Enabled)
{
    OnTrading.Broadcast(Enabled);
}

void UInventoryComponent::OpenInventory_OnClient_Implementation()
{
    const auto Character = GetOwner<ACharacter>();
    if (!Character) return;
    const auto AwesomeController = Cast<AAwesomePlayerController>(Character->GetController());
    if (AwesomeController)
    {
        AwesomeController->OpenInventory();
    }
}

void UInventoryComponent::OnMoneyChanged_OnClient_Implementation(int32 Value)
{
    OnMoneyValueChanged.Broadcast(Value);
}