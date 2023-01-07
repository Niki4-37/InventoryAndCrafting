// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AwesomeTypes.h"
#include "ItemDataWidget.generated.h"

class UBorder;
class UTexture2D;
/**
 *
 */
UCLASS()
class INVENTORYANDCRAFTING_API UItemDataWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeOnInitialized() override;

    void SetDataSlot(const FSlot& InSlotData);
    void SetIconToWidget(UTexture2D* NewIcon);

    UFUNCTION(BlueprintCallable)
    FSlot GetSlotData() const { return SlotData; };

    void SetItemIndex(uint8 Index) { ItemIndex = Index; };
    void SetSlotLocationType(ESlotLocationType Type) { LocationType = Type; };
    void SetEquipmentType(EEquipmentType Type) { EquipmentType = Type; };

    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
    virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation);

protected:
    UPROPERTY(meta = (BindWidget))
    UBorder* WidgetBorder;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UTexture2D* EmptyIcon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UUserWidget> ItemDataWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UUserWidget> ConfirmWidgetClass;

private:
    FItemData ItemData;
    FSlot SlotData;
    uint8 ItemIndex;
    ESlotLocationType LocationType;
    EEquipmentType EquipmentType;
};
