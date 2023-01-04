// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AwesomeTypes.h"
#include "AwesomePlayerController.generated.h"

class AAwesomePickupMaster;
class UAwesomeHUDWidget;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnHUDWidgetSwitchSignature, ESlateVisibility);
/**
 *
 */
UCLASS()
class INVENTORYANDCRAFTING_API AAwesomePlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    FOnHUDWidgetSwitchSignature OnHUDWidgetSwitch;

    void SpawnDroppedItem(const FSlot& DroppedItem);

    void OpenInventory();

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<AAwesomePickupMaster> PickupMasterClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UUserWidget> HUDWidgetClass;

    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

private:
    UPROPERTY()
    UAwesomeHUDWidget* HUDWidget;
};
