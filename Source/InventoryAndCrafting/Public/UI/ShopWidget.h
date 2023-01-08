// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AwesomeTypes.h"
#include "ShopWidget.generated.h"

class UUniformGridPanel;
class UButton;
/**
 *
 */
UCLASS()
class INVENTORYANDCRAFTING_API UShopWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeOnInitialized() override;

protected:
    UPROPERTY(meta = (BindWidget))
    UUniformGridPanel* ShopItemSlots;

    UPROPERTY(meta = (BindWidget))
    UButton* CloseShopButton;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UUserWidget> ItemDataWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UUserWidget> ConfirmWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (Clampmin = "1", Clampmax = "5"))
    uint8 SlotsInRow{4};

private:
    void OnNewPawn(APawn* NewPawn);
    void OnStuffEquiped(const TArray<FSlot>& Slots, ESlotLocationType Type);
    void OnSlotChanged(const FSlot& NewSlotData, const uint8 SlotIndex, ESlotLocationType Type);

    UFUNCTION()
    void OnCloseShop();
};
