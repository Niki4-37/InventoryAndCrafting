// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AwesomeTypes.h"
#include "AwesomeCraftingDeckWidget.generated.h"

class UScrollBox;
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
    UScrollBox* CraftableItemsBox;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UUserWidget> CraftableItemWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UDataTable* ItemsTable;

private:
    void InitWidget();
};
