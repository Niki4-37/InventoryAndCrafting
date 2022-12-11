// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AwesomeTypes.h"
#include "AwesomeEquipmentWidget.generated.h"

class UUniformGridPanel;
/**
 *
 */
UCLASS()
class INVENTORYANDCRAFTING_API UAwesomeEquipmentWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeOnInitialized() override;

protected:
    UPROPERTY(meta = (BindWidget))
    UUniformGridPanel* EquipmentItemSlots;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UUserWidget> ItemDataWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (Clampmin = "1", Clampmax = "10"))
    uint8 SlotsInRow{2};

private:
    void OnNewPawn(APawn* NewPawn);
    void UpdateItemSlots(const TArray<FSlot>& Slots);
};
