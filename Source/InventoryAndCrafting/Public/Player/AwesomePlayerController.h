// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AwesomeTypes.h"
#include "AwesomePlayerController.generated.h"

class UAwesomeHUDWidget;

/**
 *
 */
UCLASS()
class INVENTORYANDCRAFTING_API AAwesomePlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    void OpenInventory();
    void CloseInventory();

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UUserWidget> HUDWidgetClass;

    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    virtual void OnPossess(APawn* aPawn) override;

private:
    UPROPERTY()
    UAwesomeHUDWidget* HUDWidget;

    void OpenCloseInventory();
};
