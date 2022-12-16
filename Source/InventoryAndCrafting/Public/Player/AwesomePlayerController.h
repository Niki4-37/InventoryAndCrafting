// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AwesomeTypes.h"
#include "AwesomePlayerController.generated.h"

class AAwesomePickupMaster;
/**
 *
 */
UCLASS()
class INVENTORYANDCRAFTING_API AAwesomePlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    void SpawnDroppedItem(const FSlot& DroppedItem);

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<AAwesomePickupMaster> PickupMasterClass;
};
