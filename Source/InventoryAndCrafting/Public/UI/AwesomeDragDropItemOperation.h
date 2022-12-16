// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "AwesomeTypes.h"
#include "AwesomeDragDropItemOperation.generated.h"

/**
 *
 */
UCLASS()
class INVENTORYANDCRAFTING_API UAwesomeDragDropItemOperation : public UDragDropOperation
{
    GENERATED_BODY()

public:
    void SetSlotData(const FSlot& InSlotData) { SlotData = InSlotData; };
    void SetSlotIndex(const uint8 InSlotIndex) { SlotIndex = InSlotIndex; };

    FSlot GetSlotData() const { return SlotData; };
    uint8 GetSlotIndex() const { return SlotIndex; };

private:
    FSlot SlotData;
    uint8 SlotIndex;
};
