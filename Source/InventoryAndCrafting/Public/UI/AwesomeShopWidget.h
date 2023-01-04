// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AwesomeTypes.h"
#include "AwesomeShopWidget.generated.h"

class UUniformGridPanel;
/**
 *
 */
UCLASS()
class INVENTORYANDCRAFTING_API UAwesomeShopWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeOnInitialized() override;

    virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation);

protected:
    UPROPERTY(meta = (BindWidget))
    UUniformGridPanel* ShopItemSlots;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UUserWidget> ItemDataWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (Clampmin = "1", Clampmax = "5"))
    uint8 SlotsInRow{4};

private:
    void OnNewPawn(APawn* NewPawn);
    void OnStuffEquiped(const TArray<FSlot>& Slots, ESlotLocationType Type);
    void OnSlotChanged(const FSlot& NewSlotData, const uint8 SlotIndex, ESlotLocationType Type);
};
