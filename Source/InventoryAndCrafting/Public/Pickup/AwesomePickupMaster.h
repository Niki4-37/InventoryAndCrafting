// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "AwesomeTypes.h"
#include "Interfaces/AwesomeInteractionInterface.h"
#include "AwesomePickupMaster.generated.h"

/**
 *
 */
UCLASS()
class INVENTORYANDCRAFTING_API AAwesomePickupMaster : public AStaticMeshActor, public IAwesomeInteractionInterface
{
    GENERATED_BODY()

public:
    AAwesomePickupMaster();

    FSlot GetPickupItem() const { return PickupItem; };
    void SetPickupItem(const FSlot& InItemData) { PickupItem = InItemData; };

    /* Interface function */
    virtual void Interact(AActor* InteractiveActor) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FSlot PickupItem;

    virtual void BeginPlay() override;

private:
    void InitPickup();
};
