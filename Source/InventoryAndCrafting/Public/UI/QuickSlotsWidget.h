// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AwesomeTypes.h"
#include "QuickSlotsWidget.generated.h"

class UUniformGridPanel;
/**
 *
 */
UCLASS()
class INVENTORYANDCRAFTING_API UQuickSlotsWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeOnInitialized() override;

protected:
    UPROPERTY(meta = (BindWidget))
    UUniformGridPanel* QuickSlots;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UUserWidget> DisplayingItemWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (Clampmin = "1", Clampmax = "10"))
    uint8 SlotsInRow{8};

private:
    void OnNewPawn(APawn* NewPawn);
    void OnQuickSlotsCreated(const TArray<FQuickSlot>& Slots);
    void OnQuickSlotsDataChanged(const FQuickSlot& Data, uint8 Index);
};
