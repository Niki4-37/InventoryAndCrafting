// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AwesomeTypes.h"
#include "DisplayingItemWidget.generated.h"

class UTexture2D;
class UBorder;
class UTextBlock;
/**
 *
 */
UCLASS()
class INVENTORYANDCRAFTING_API UDisplayingItemWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    void SetDisplayingInfo(UTexture2D* NewIcon, int32 InAmount = 0);

    void SetQuickSlotData(const FQuickSlot& Data) { QuickSlotData = Data; };
    void SetWidgetIndex(uint8 InWidgetIndex) { WidgetIndex = InWidgetIndex; };

protected:
    UPROPERTY(meta = (BindWidget))
    UBorder* DisplayingBorder;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UTexture2D* EmptyIcon;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* AmountText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* KeyboardKeyText;

private:
    uint8 WidgetIndex;
    FQuickSlot QuickSlotData;
};
