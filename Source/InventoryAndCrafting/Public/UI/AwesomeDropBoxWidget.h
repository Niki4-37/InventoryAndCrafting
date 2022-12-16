// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AwesomeTypes.h"
#include "AwesomeDropBoxWidget.generated.h"

class UBorder;
/**
 *
 */
UCLASS()
class INVENTORYANDCRAFTING_API UAwesomeDropBoxWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation);

protected:
    UPROPERTY(meta = (BindWidget))
    UBorder* DropArea;

private:
    void DropFromInventory(const uint8 Index, const FSlot& DroppedItem);
    void DropFromEquipment(const uint8 Index, const FSlot& DroppedItem);
};
