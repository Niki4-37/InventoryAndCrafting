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

DECLARE_MULTICAST_DELEGATE(FOnStuffEquipedSignature);

UCLASS()
class INVENTORYANDCRAFTING_API AAwesomeBaseCharacter : public ACharacter, public IAwesomeInteractionInterface
{
    GENERATED_BODY()

public:
    AAwesomeBaseCharacter();

    /* used in widget */
    FOnSlotsChangedSignature OnSlotsChanged;
    FOnStuffEquipedSignature OnStuffEquiped;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    USpringArmComponent* SpringArmComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UCameraComponent* CameraComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Clampmin = "0", Clampmax = "20"))
    uint8 EquipmentSlots{4};

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    FName BackpackSocketName;

    virtual void BeginPlay() override;

public:
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    TArray<FSlot> GetSlots() const { return Slots; };

    bool FindStackOfSameItems(const FSlot& Item, uint8& OutSlotIndex, int32& OutAmount, bool& bOutCanStack);
    bool FindEmptySlot(uint8& OutSlotIndex);
    bool RemoveAmountFromSlotsAtIndex(const uint8 Index, const int32 AmountToRemove);
    bool RemoveItemFromSlots(const FSlot& Item);
    bool TryAddItemToSlots(const FSlot& Item);

    AAwesomeBackpackMaster* GetBackpack() const { return EquipedBackpack; };
    void EquipBackpack(AAwesomeBackpackMaster* Backpack);
    TArray<FSlot> GetBackpackSlots() const;

private:
    TArray<FSlot> Slots;

    UPROPERTY()
    AAwesomeBackpackMaster* EquipedBackpack{nullptr};

    void InitEquipment();

    void MoveForward(float Amount);
    void MoveRight(float Amount);

    void TakeItem();

    bool UpdateSlotItemData(const uint8 Index, const int32 AmountModifier);
};
