// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Interfaces/AwesomeInteractionInterface.h"
#include "AwesomeTypes.h"
#include "AwesomeBackpackMaster.generated.h"

/**
 *
 */
UCLASS()
class INVENTORYANDCRAFTING_API AAwesomeBackpackMaster : public AStaticMeshActor, public IAwesomeInteractionInterface
{
    GENERATED_BODY()

public:
    AAwesomeBackpackMaster();
    FOnSlotDataChangedSignature OnSlotChanged;

    virtual void Interact(AActor* InteractiveActor) override;

    TArray<FSlot> GetBackpackSlots() const { return BackpackSlots; };
    bool FindStackOfSameItems(const FSlot& Item, uint8& OutSlotIndex, int32& OutAmount, bool& bOutCanStack);
    bool FindEmptySlot(uint8& OutSlotIndex);
    bool TryAddItemToSlots(const FSlot& Item);
    bool RemoveAmountFromInventorySlotsAtIndex(const uint8 Index, const int32 AmountToRemove);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Clampmin = "1", Clampmax = "25"))
    uint8 BackpackSlotsNumber{15};

    virtual void BeginPlay() override;

private:
    TArray<FSlot> BackpackSlots;

    void InitBackpack();
    bool UpdateSlotItemData(const uint8 Index, const int32 AmountModifier);
};
