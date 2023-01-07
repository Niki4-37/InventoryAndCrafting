// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AwesomeTypes.h"
#include "CraftableItemWidget.generated.h"

class UBorder;
class UTextBlock;
class UInventoryComponent;
class AAwesomeBackpackMaster;

USTRUCT()
struct FCraftingSet
{
    GENERATED_BODY()

public:
    uint8 ComponentIndex;
    int32 ComponentValue;

    FCraftingSet()
    {
        ComponentIndex = 0;
        ComponentValue = 0;
    }
    FCraftingSet(uint8 Index, int32 Value) : ComponentIndex(Index), ComponentValue(Value) {}
    FCraftingSet(const FCraftingSet& Set) : ComponentIndex(Set.ComponentIndex), ComponentValue(Set.ComponentValue) {}
};

UCLASS()
class INVENTORYANDCRAFTING_API UCraftableItemWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void InitWidget(UTexture2D* Icon, int32 Amount, const FName& InItemName, const TArray<FSlot> InRecipe);
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    UFUNCTION(BlueprintCallable)
    TArray<FSlot> GetRecipe() const { return Recipe; };

protected:
    UPROPERTY(meta = (BindWidget))
    UBorder* CraftingBorder;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* OutCraftingAmountText;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UDataTable* ItemsTable;

private:
    int32 CraftingOutAmount;
    TArray<FSlot> Recipe;
    FName ItemName;

    TArray<FCraftingSet> ComponentsFromRecipe;

    bool CheckForCraftingComponents(UInventoryComponent* Inventory);
    void CraftTheItem();

    void SpendComponents(AAwesomeBackpackMaster* Backpack);
};
