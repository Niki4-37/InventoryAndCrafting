// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AwesomeTypes.h"
#include "AwesomeCraftingDeckWidget.generated.h"

class UUniformGridPanel;
class UDataTable;
/**
 *
 */
UCLASS()
class INVENTORYANDCRAFTING_API UAwesomeCraftingDeckWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeOnInitialized() override;

protected:
    UPROPERTY(meta = (BindWidget))
    UUniformGridPanel* CraftableItemsBox;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (Clampmin = "1", Clampmax = "10"))
    uint8 SlotsInRow{4};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UUserWidget> CraftableItemWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UDataTable* ItemsTable;

private:
    void InitWidget();
};
