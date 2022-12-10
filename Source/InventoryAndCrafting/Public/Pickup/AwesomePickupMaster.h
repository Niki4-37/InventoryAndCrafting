// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "AwesomeTypes.h"
#include "AwesomePickupMaster.generated.h"

/**
 *
 */
UCLASS()
class INVENTORYANDCRAFTING_API AAwesomePickupMaster : public AStaticMeshActor
{
    GENERATED_BODY()

public:
    AAwesomePickupMaster();

    FSlot GetPickupItem() const { return PickupItem; };

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FSlot PickupItem;

    virtual void BeginPlay() override;

private:
    void InitPickup();
};
