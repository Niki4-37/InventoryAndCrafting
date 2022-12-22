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

UCLASS()
class INVENTORYANDCRAFTING_API AAwesomeBaseCharacter : public ACharacter, public IAwesomeInteractionInterface
{
    GENERATED_BODY()

public:
    AAwesomeBaseCharacter();

    /* used in widget */
    FOnSlotDataChangedSignature OnSlotChanged;
    FOnStuffEquipedSignature OnStuffEquiped;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    USpringArmComponent* SpringArmComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UCameraComponent* CameraComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Clampmin = "0", Clampmax = "20"))
    uint8 PersonalSlotsNumber{4};

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    FName BackpackSocketName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    FName ArmorSocketName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TSubclassOf<AActor> ArmorClass;

    virtual void BeginPlay() override;

public:
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    TArray<FSlot> GetPersonalSlots() const { return PersonalSlots; };

    bool TryAddItemToSlots(const FSlot& Item);
    bool TryAddItemToPersonalSlotsByIndex(const FSlot& Item, const uint8 InIndex);

    bool MoveItem(const FSlot& Item, ESlotLocationType FromLocationType, const uint8 FromSlotIndex, ESlotLocationType ToLocationType, const uint8 ToSlotIndex);

    AAwesomeBackpackMaster* GetBackpack() const { return EquipedBackpack; };
    void EquipBackpack(AAwesomeBackpackMaster* Backpack);
    TArray<FSlot> GetBackpackSlots() const;

private:
    TArray<FSlot> PersonalSlots;
    TArray<FSlot> EquipmentSlots;

    UPROPERTY()
    AAwesomeBackpackMaster* EquipedBackpack{nullptr};

    void InitPersonalSlots();

    void MoveForward(float Amount);
    void MoveRight(float Amount);

    bool FindStackOfSameItems(const FSlot& Item, uint8& OutSlotIndex, int32& OutAmount, bool& bOutCanStack);
    bool FindEmptySlot(uint8& OutSlotIndex);

    bool RemoveAmountFromChoosenSlotsAtIndex(TArray<FSlot>& Slots, const uint8 Index, const int32 AmountToRemove);
    bool RemoveItemFromPersonalSlots(const FSlot& Item);

    // bool TryAddItemToEquipment();

    void TakeItem();

    bool UpdateSlotItemData(TArray<FSlot>& Slots, const uint8 Index, const int32 AmountModifier);

    void EquipArmor();
};
