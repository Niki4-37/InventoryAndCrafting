// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AwesomeTypes.h"
#include "AwesomeEquipmentWidget.generated.h"

class UUniformGridPanel;
class USizeBox;
class UAwesomeItemDataWidget;
/**
 *
 */
UCLASS()
class INVENTORYANDCRAFTING_API UAwesomeEquipmentWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeOnInitialized() override;

protected:
    UPROPERTY(meta = (BindWidget))
    USizeBox* HeadSlotBox;
    UPROPERTY(meta = (BindWidget))
    USizeBox* BackSlotBox;
    UPROPERTY(meta = (BindWidget))
    USizeBox* RightArmSlotBox;
    UPROPERTY(meta = (BindWidget))
    USizeBox* TorsoSlotBox;
    UPROPERTY(meta = (BindWidget))
    USizeBox* LeftArmSlotBox;
    UPROPERTY(meta = (BindWidget))
    USizeBox* LegsSlotBox;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UUserWidget> ItemDataWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TMap<EEquipmentType, UTexture2D*> DefaultEqiupmentIconsMap;

private:
    UPROPERTY()
    TMap<EEquipmentType, UAwesomeItemDataWidget*> EqiupmentSlotsMap;

    void InitEquipment();
    void InitEquipmentSlot(USizeBox* Box, UAwesomeItemDataWidget* ItemWidget, EEquipmentType Type);

    void OnNewPawn(APawn* NewPawn);
    void OnEquipmentSlotDataChanged(const FSlot& NewSlotData, EEquipmentType Type);
};
