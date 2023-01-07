// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AwesomeTypes.h"
#include "InventoryComponent.generated.h"

class AAwesomeBackpackMaster;
class AAwesomePickupMaster;
class AAwesomeShop;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class INVENTORYANDCRAFTING_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInventoryComponent();

    /* used in widget */
    FOnSlotDataChangedSignature OnSlotChanged;
    FOnStuffEquipedSignature OnStuffEquiped;
    FOnEquipmentSlotDataChangedSignature OnEquipmentSlotDataChanged;
    FOnTradingSignature OnTrading;
    FOnMoneyValueChangedSignature OnMoneyValueChanged;

    UFUNCTION(Server, Reliable)
    void EquipBackpack_OnServer(AAwesomeBackpackMaster* Backpack);

    UFUNCTION(Server, Unreliable)
    void StartTrading_OnServer(AAwesomeShop* Shop);

    UFUNCTION(Server, Reliable)
    void StopTrading_OnServer();

    AAwesomeBackpackMaster* GetBackpack() const { return EquipedBackpack; };

    UFUNCTION(Server, Reliable)
    void PickupItem_OnServer(AAwesomePickupMaster* Pickup);

    UFUNCTION(Server, Reliable)
    void MoveItem_OnServer(const FSlot& Item,                   //
                           ESlotLocationType FromLocationType,  //
                           EEquipmentType FromEquipmentType,    //
                           const uint8 FromSlotIndex,           //
                           ESlotLocationType ToLocationType,    //
                           EEquipmentType ToEquipmentType,      //
                           const uint8 ToSlotIndex);

    void UpdateWidgetSlotData(const FSlot& Item, const uint8 Index, ESlotLocationType Type);

    void UpdateShopWidgetAfterTransaction(const TArray<FSlot>& Goods);

    bool HasEquipmentToSwap(EEquipmentType FirstSlotType, EEquipmentType SecondSlotType);
    void SwapItems(EEquipmentType FirstSlotType, EEquipmentType SecondSlotType);

    bool DropItem(const FSlot& Item);

protected:
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (Clampmin = "0", Clampmax = "20"))
    uint8 PersonalSlotsNumber{4};

    UPROPERTY(Replicated /*Using = MoneyChanged_OnRep*/, EditDefaultsOnly, BlueprintReadWrite)
    int32 Money;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<AAwesomePickupMaster> PickupMasterClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    FName BackpackSocketName;

    UPROPERTY(VisibleAnywhere)
    TMap<EEquipmentType, FName> EquipmentSocketNamesMap;

    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    UPROPERTY(Replicated)
    TArray<FSlot> PersonalSlots;
    UPROPERTY(Replicated)
    TArray<FEquipmentSlot> EquipmentSlots;

    UPROPERTY(Replicated)
    AAwesomeBackpackMaster* EquipedBackpack{nullptr};

    UPROPERTY(Replicated)
    AAwesomeShop* ActiveShop{nullptr};

    UPROPERTY()
    TMap<EEquipmentType, AActor*> EquippedItemsMap;

    UFUNCTION(Server, Reliable)
    void InitEnableSlots_OnServer();

    bool FindStackOfSameItems(const FSlot& Item, uint8& OutSlotIndex, int32& OutAmount, bool& bOutCanStack);
    bool FindEmptySlot(uint8& OutSlotIndex);

    bool TryAddItemToSlots(const FSlot& Item);
    bool TryAddItemToPersonalSlotsByIndex(const FSlot& Item, const uint8 InIndex);
    bool TryAddItemToEquipment(const FSlot& Item, EEquipmentType ToEquipmentType);
    bool CheckForWeapon(EEquipmentType& WeaponEquipmentType, EEquipmentType ToEquipmentType);

    bool RemoveAmountFromChoosenSlotsAtIndex(TArray<FSlot>& Slots, const uint8 Index, const int32 AmountToRemove);
    bool RemoveItemFromPersonalSlots(const FSlot& Item);
    FSlot RemoveItemFromEquipment(EEquipmentType FromEquipmentType);

    void RemovePersonalExtraSlots(const FItemData& ItemData);

    bool UpdateSlotItemData(TArray<FSlot>& Slots, const uint8 Index, const int32 AmountModifier);

    void EquipItem(UClass* Class, UStaticMesh* NewMesh, FName SocketName, EEquipmentType Type);

    bool TrySellItem(const FSlot& SellingItem);
    void TryBuyItem(const FSlot& BuyingItem, uint8 Index, uint8 ToSlotNumber, ESlotLocationType ToLocationType, EEquipmentType ToEquipmentType);

    UFUNCTION(Client, Reliable)
    void OnStuffEquiped_OnClient(const TArray<FSlot>& Slots, ESlotLocationType Type);

    UFUNCTION(Client, Reliable)
    void OnSlotChanged_OnClient(const FSlot& Item, const uint8 Index, ESlotLocationType Type);

    UFUNCTION(Client, Reliable)
    void OnEquipmentSlotDataChanged_OnClient(const FSlot& Item, EEquipmentType Type);

    UFUNCTION(NetMulticast, Reliable)
    void SetStaticMesh_Multicast(AAwesomeEquipmentActor* Actor, UStaticMesh* NewMesh);

    UFUNCTION(Client, unreliable)
    void OnTrading_OnClient(bool Enabled);

    // UFUNCTION()
    // void MoneyChanged_OnRep();

    UFUNCTION(Client, unreliable)
    void OnMoneyChanged_OnClient(int32 Value);
};
