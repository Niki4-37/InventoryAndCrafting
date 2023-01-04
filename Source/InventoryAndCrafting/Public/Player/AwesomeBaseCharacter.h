// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AwesomeTypes.h"
#include "Interfaces/AwesomeInteractionInterface.h"
#include "AwesomeBaseCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class AAwesomeBackpackMaster;
class AAwesomePickupMaster;
class AAwesomeShop;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTradingSignature, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMoneyValueChangedSignature, int32);

UCLASS()
class INVENTORYANDCRAFTING_API AAwesomeBaseCharacter : public ACharacter, public IAwesomeInteractionInterface
{
    GENERATED_BODY()

public:
    AAwesomeBaseCharacter();

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

    AAwesomeBackpackMaster* GetBackpack() const { return EquipedBackpack; };

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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

    // Testing
    void UpdateShopWidgetAfterTransaction(const TArray<FSlot>& Goods);

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    USpringArmComponent* SpringArmComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UCameraComponent* CameraComponent;

    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (Clampmin = "0", Clampmax = "20"))
    uint8 PersonalSlotsNumber{4};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UAnimMontage* SwapWeaponsMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UAnimMontage* DrawWeaponMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    FName BackpackSocketName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TMap<EEquipmentType, FName> EquipmentSocketNamesMap;

    UPROPERTY(Replicated /*Using = MoneyChanged_OnRep*/, EditDefaultsOnly, BlueprintReadWrite)
    int32 Money;

    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    UPROPERTY(Replicated)
    TArray<FSlot> PersonalSlots;
    UPROPERTY(Replicated)
    TArray<FEquipmentSlot> EquipmentSlots;
    UPROPERTY(Replicated)
    bool bIsPlayingMontage{false};

    UPROPERTY()
    TMap<EEquipmentType, AActor*> EquippedItemsMap;

    UPROPERTY(Replicated)
    AAwesomeBackpackMaster* EquipedBackpack{nullptr};

    FTimerHandle PlayerActionTimer;

    UPROPERTY(Replicated)
    AAwesomeShop* ActiveShop{nullptr};

    UFUNCTION(Server, Reliable)
    void InitEnableSlots_OnServer();

    void MoveForward(float Amount);
    void MoveRight(float Amount);

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

    bool HasEquipmentToSwap(EEquipmentType FirstSlotType, EEquipmentType SecondSlotType);

    UFUNCTION(Server, Reliable)
    void SwapWeapons_OnServer();
    UFUNCTION(Server, Reliable)
    void DrawWeapon_OnServer();

    void SwapWeapons();
    void DrawWeapon();

    void TakeItem();

    bool UpdateSlotItemData(TArray<FSlot>& Slots, const uint8 Index, const int32 AmountModifier);

    void EquipItem(UClass* Class, UStaticMesh* NewMesh, FName SocketName, EEquipmentType Type);

    bool TrySellItem(const FSlot& SellingItem);
    void BuyItem(const FSlot& BuyingItem, uint8 Index);

    void OnHUDWidgetSwitch(ESlateVisibility Visibility);

    UFUNCTION(Client, Reliable)
    void OnStuffEquiped_OnClient(const TArray<FSlot>& Slots, ESlotLocationType Type);

    UFUNCTION(Client, Reliable)
    void OnSlotChanged_OnClient(const FSlot& Item, const uint8 Index, ESlotLocationType Type);

    UFUNCTION(Client, Reliable)
    void OnEquipmentSlotDataChanged_OnClient(const FSlot& Item, EEquipmentType Type);

    UFUNCTION(NetMulticast, Reliable)
    void SetStaticMesh_Multicast(AAwesomeEquipmentActor* Actor, UStaticMesh* NewMesh);

    UFUNCTION(NetMulticast, Unreliable)
    void PlayAnimMontage_Multicast(UAnimMontage* Montage, float PlayRate);

    UFUNCTION(Client, unreliable)
    void OnTrading_OnClient(bool Enabled);

    UFUNCTION(Client, unreliable)
    void OpenInventory_OnClient();

    UFUNCTION(Server, Reliable)
    void StopTrading_OnServer();

    // UFUNCTION()
    // void MoneyChanged_OnRep();

    UFUNCTION(Client, unreliable)
    void OnMoneyChanged_OnClient(int32 Value);

    UFUNCTION(Server, unreliable)
    void SwitchMovement_OnServer(bool bCanMove);
};