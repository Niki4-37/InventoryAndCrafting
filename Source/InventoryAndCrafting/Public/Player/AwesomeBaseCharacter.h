// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AwesomeTypes.h"
#include "AwesomeBaseCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSlotsChangedSignature, const TArray<FSlot>&);

UCLASS()
class INVENTORYANDCRAFTING_API AAwesomeBaseCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AAwesomeBaseCharacter();

    FOnSlotsChangedSignature OnSlotsChanged;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    USpringArmComponent* SpringArmComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UCameraComponent* CameraComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Clampmin = "0", Clampmax = "20"))
    uint8 EquipmentSlots{4};

    virtual void BeginPlay() override;

public:
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    TArray<FSlot> GetSlots() const { return Slots; };

    bool FindStackOfSameItems(const FSlot& Item, uint8& OutSlotIndex, int32& OutAmount, bool& bOutCanStack);
    bool FindEmptySlot(uint8& OutSlotIndex);
    bool RemoveAmountFromSlotsAtIndex(const uint8 Index, const int32 AmountToRemove);
    bool RemoveItemFromSlots(const FSlot& Item);
    bool TryAddItemToSlots(const FSlot& Item);

private:
    TArray<FSlot> Slots;
    void InitEquipment();

    void MoveForward(float Amount);
    void MoveRight(float Amount);

    void TakeItem();

    bool UpdateSlotItemData(const uint8 Index, const int32 AmountModifier);
};
