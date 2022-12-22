// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AwesomeTypes.h"
#include "AwesomeInventoryWidget.generated.h"

class UUniformGridPanel;
/**
 *
 */
UCLASS()
class INVENTORYANDCRAFTING_API UAwesomeInventoryWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeOnInitialized() override;

protected:
    UPROPERTY(meta = (BindWidget))
    UUniformGridPanel* InventoryItemSlots;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UUserWidget> ItemDataWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (Clampmin = "1", Clampmax = "10"))
    uint8 SlotsInRow{3};

private:
    void OnNewPawn(APawn* NewPawn);
    void OnStuffEquiped(const TArray<FSlot>& Slots, ESlotLocationType Type);
    void OnSlotChanged(const FSlot& NewSlotData, const uint8 SlotIndex);
};
