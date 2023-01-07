// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AwesomeTypes.h"
#include "ConfirmWidget.generated.h"

class UButton;
// class UTexture2D;
class UBorder;
/**
 *
 */
UCLASS()
class INVENTORYANDCRAFTING_API UConfirmWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeOnInitialized() override;

    void SetSlotData(const FSlot& Data);
    void SetDraDropData(const FDragDropData& Data) { DragDropData = Data; };

protected:
    UPROPERTY(meta = (BindWidget))
    UButton* ConfirmButton;

    UPROPERTY(meta = (BindWidget))
    UButton* CancelButton;

    UPROPERTY(meta = (BindWidget))
    UBorder* IconBorder;

private:
    FSlot SlotData;
    FDragDropData DragDropData;

    UFUNCTION()
    void OnCorfirm();

    UFUNCTION()
    void OnCancel();
};
