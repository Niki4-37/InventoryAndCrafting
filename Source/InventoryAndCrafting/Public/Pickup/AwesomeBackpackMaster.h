// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Interfaces/AwesomeInteractionInterface.h"
#include "AwesomeTypes.h"
#include "AwesomeBackpackMaster.generated.h"

/**
 *
 */
UCLASS()
class INVENTORYANDCRAFTING_API AAwesomeBackpackMaster : public AStaticMeshActor, public IAwesomeInteractionInterface
{
    GENERATED_BODY()

public:
    AAwesomeBackpackMaster();

    virtual void Interact(AActor* InteractiveActor) override;

    TArray<FSlot> GetBackpackSlots() const { return BackpackSlots; };
    bool FindStackOfSameItems(const FSlot& Item, uint8& OutSlotIndex, int32& OutAmount, bool& bOutCanStack);
    bool FindEmptySlot(uint8& OutSlotIndex);
    bool TryAddItemToSlots(const FSlot& Item);
    bool RemoveAmountFromInventorySlotsAtIndex(const uint8 Index, const int32 AmountToRemove);

protected:
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (Clampmin = "1", Clampmax = "25"))
    uint8 BackpackSlotsNumber{15};

    virtual void BeginPlay() override;
    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    UPROPERTY(Replicated)
    TArray<FSlot> BackpackSlots;

    UFUNCTION(Server, Reliable)
    void InitBackpack_OnServer();

    bool UpdateSlotItemData(const uint8 Index, const int32 AmountModifier);

    void UpdateOwnerWidget(const FSlot& Item, const uint8 Index);
};
