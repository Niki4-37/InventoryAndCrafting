// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AwesomeTypes.h"
#include "AwesomeItemDataWidget.generated.h"

class UBorder;
class UTexture2D;
/**
 *
 */
UCLASS()
class INVENTORYANDCRAFTING_API UAwesomeItemDataWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeOnInitialized() override;

    void SetDataFromSlot(const FSlot& InSlotData);

    UFUNCTION(BlueprintCallable)
    FSlot GetSlotData() const { return SlotData; };

    void SetItemIndex(uint8 Index) { ItemIndex = Index; };

    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

protected:
    UPROPERTY(meta = (BindWidget))
    UBorder* WidgetBorder;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UTexture2D* EmptyIcon;

private:
    FItemData ItemData;
    FSlot SlotData;
    uint8 ItemIndex;

    void SetIconToWidget();
};
