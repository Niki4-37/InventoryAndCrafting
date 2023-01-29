// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AwesomeTypes.h"
#include "PlayerHUDWidget.generated.h"

class UWidgetSwitcher;
class UCanvasPanel;
class UBorder;
class UButton;
class UProgressBar;
/**
 *
 */
UCLASS()
class INVENTORYANDCRAFTING_API UPlayerHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeOnInitialized() override;
    void ShowInventory(bool bEnabled);
    bool GetInventoryVisibility() const { return bIsVisible; };

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
    UBorder* InventoryPosition;
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
    UBorder* QuickSlotsPosition;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UUserWidget> QuickSlotsWidgetClass;

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

    UPROPERTY(meta = (BindWidget))
    UProgressBar* HealthBar;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* ThirstBar;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* HungerBar;

private:
    bool bIsVisible;

    void OnNewPawn(APawn* NewPawn);
    void InitWidget();
    UFUNCTION()
    void OnSwitchCraftingPanel();
    UFUNCTION()
    void OnGoBack();
    void OnTrading(bool Enable);
    void OnVitalParameterChanged(EVitalParameterType Type, float Persentage);
};
