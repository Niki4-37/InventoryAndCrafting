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

    void SetDataFromItem(const FSlot& ItemData);

    UFUNCTION(BlueprintCallable)
    FSlot GetItemData() const { return Item; };

protected:
    UPROPERTY(meta = (BindWidget))
    UBorder* WidgetBorder;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UTexture2D* EmptyIcon;

    FInventoryData InventoryData;
    FSlot Item;

private:
    void SetIconToWidget();
};
