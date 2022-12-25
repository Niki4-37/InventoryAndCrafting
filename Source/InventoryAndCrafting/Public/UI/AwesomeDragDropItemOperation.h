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
    void SetFromSlotIndex(const uint8 InIndex) { FromSlotIndex = InIndex; };
    void SetFromSlotLocationType(ESlotLocationType InType) { FromLocationType = InType; };
    void SetFromEquipmentType(EEquipmentType InType) { FromEquipmentType = InType; };

    FSlot GetSlotData() const { return SlotData; };
    uint8 GetFromSlotIndex() const { return FromSlotIndex; };
    ESlotLocationType GetItemFromLocationType() const { return FromLocationType; };
    EEquipmentType GetFromEquipmentType() const { return FromEquipmentType; };

private:
    FSlot SlotData;
    ESlotLocationType FromLocationType;
    EEquipmentType FromEquipmentType;
    uint8 FromSlotIndex;
};
