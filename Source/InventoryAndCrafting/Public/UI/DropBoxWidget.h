// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AwesomeTypes.h"
#include "DropBoxWidget.generated.h"

class UBorder;
/**
 *
 */
UCLASS()
class INVENTORYANDCRAFTING_API UDropBoxWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation);

protected:
    UPROPERTY(meta = (BindWidget))
    UBorder* DropArea;
};
