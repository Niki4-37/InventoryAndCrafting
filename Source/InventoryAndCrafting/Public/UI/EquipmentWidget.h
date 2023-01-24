// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AwesomeTypes.h"
#include "EquipmentWidget.generated.h"

class UUniformGridPanel;
class USizeBox;
class UItemDataWidget;
class UTextBlock;
class APreviewPlayer;
/**
 *
 */
UCLASS()
class INVENTORYANDCRAFTING_API UEquipmentWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeOnInitialized() override;

    UFUNCTION(BlueprintCallable)
    UMaterialInstanceDynamic* GetMaterialInstance() const { return RenderTargetMaterial; };

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

    UPROPERTY(meta = (BindWidget))
    UTextBlock* MoneyValueText;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UUserWidget> ItemDataWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TMap<EEquipmentType, UTexture2D*> DefaultEqiupmentIconsMap;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<AActor> PreviewPlayerClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector PreviewPlayerSpawnLocation;

private:
    UPROPERTY()
    TMap<EEquipmentType, UItemDataWidget*> EqiupmentSlotsMap;

    UPROPERTY()
    UMaterialInstanceDynamic* RenderTargetMaterial;

    UPROPERTY()
    APreviewPlayer* PreviewPlayer;

    void InitEquipment();
    void InitEquipmentSlot(USizeBox* Box, UItemDataWidget* ItemWidget, EEquipmentType Type);

    void OnNewPawn(APawn* NewPawn);
    void OnEquipmentSlotDataChanged(const FSlot& NewSlotData, EEquipmentType Type, UStaticMesh* NewMesh);
    void OnMoneyValueChanged(int32 Value);
};
