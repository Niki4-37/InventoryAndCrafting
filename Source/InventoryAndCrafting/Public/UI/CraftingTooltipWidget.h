// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AwesomeTypes.h"
#include "CraftingTooltipWidget.generated.h"

class UHorizontalBox;
/**
 *
 */
UCLASS() class INVENTORYANDCRAFTING_API UCraftingTooltipWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void InitWidget(const TArray<FSlot> InRecipe);

protected:
    UPROPERTY(meta = (BindWidget))
    UHorizontalBox* CraftingRecipeBox;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UUserWidget> CraftableItemWidgetClass;
};
