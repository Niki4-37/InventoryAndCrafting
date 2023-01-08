// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "AwesomeTypes.h"
#include "Interfaces/InteractionInterface.h"
#include "PickupMaster.generated.h"

class UBoxComponent;
/**
 *
 */
UCLASS()
class INVENTORYANDCRAFTING_API APickupMaster : public AStaticMeshActor, public IInteractionInterface
{
    GENERATED_BODY()

public:
    APickupMaster();

    FSlot GetPickupItem() const { return PickupItem; };
    void SetPickupItem(const FSlot& InItemData) { PickupItem = InItemData; };

    /* Interface function */
    virtual void Interact(AActor* InteractiveActor) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    UPROPERTY(ReplicatedUsing = PickupItem_OnRep, EditAnywhere, BlueprintReadWrite)
    FSlot PickupItem;

    UPROPERTY(EditAnywhere)
    UBoxComponent* BoxComponent;

    virtual void BeginPlay() override;

private:
    void InitPickup();

    UFUNCTION()
    void PickupItem_OnRep() { InitPickup(); }
};
