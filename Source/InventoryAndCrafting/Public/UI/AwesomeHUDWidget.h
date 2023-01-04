// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AwesomeHUDWidget.generated.h"

class UWidgetSwitcher;
class UCanvasPanel;
class UBorder;
class UButton;
/**
 *
 */
UCLASS()
class INVENTORYANDCRAFTING_API UAwesomeHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeOnInitialized() override;

protected:
    UPROPERTY(meta = (BindWidget))
    UWidgetSwitcher* WidgetSwitcherBetween;

    UPROPERTY(meta = (BindWidget))
    UCanvasPanel* InventoryAndEquipmentPanel;

    UPROPERTY(meta = (BindWidget))
    UCanvasPanel* CraftingPanel;

    UPROPERTY(meta = (BindWidget))
    UBorder* DropBoxPosition;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UUserWidget> DropBoxWidgetClass;

    UPROPERTY(meta = (BindWidget))
    UBorder* InventoriPosition;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UUserWidget> InventoriWidgetClass;

    UPROPERTY(meta = (BindWidget))
    UBorder* EquipmentPosition;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UUserWidget> EquipmentWidgetClass;

    UPROPERTY(meta = (BindWidget))
    UBorder* PersonalSlotsPosition;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UUserWidget> PersonalSlotsWidgetClass;

    UPROPERTY(meta = (BindWidget))
    UBorder* CraftingDeckPosition;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UUserWidget> CraftingDeckWidgetClass;

    UPROPERTY(meta = (BindWidget))
    UBorder* ShopPosition;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UUserWidget> ShopWidgetClass;

    UPROPERTY(meta = (BindWidget))
    UButton* SwitchToCraftingButton;

    UPROPERTY(meta = (BindWidget))
    UButton* BackButton;

private:
    void OnNewPawn(APawn* NewPawn);
    void InitWidget();
    UFUNCTION()
    void OnSwitchCraftingPanel();
    UFUNCTION()
    void OnGoBack();
    void OnTrading(bool Enable);
};
