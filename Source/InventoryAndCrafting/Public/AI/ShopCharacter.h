// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AwesomeTypes.h"
#include "Interfaces/InteractionInterface.h"
#include "ShopCharacter.generated.h"

UCLASS()
class INVENTORYANDCRAFTING_API AShopCharacter : public ACharacter, public IInteractionInterface
{
    GENERATED_BODY()

public:
    AShopCharacter();

    TArray<FSlot> GetGoods() const { return ShopSlots; };

    void AddBuyer(AActor* Buyer);
    void StopTrading(AActor* Buyer);
    void BuyItemInStore(uint8 Index);
    bool SellItemInStore(const FSlot& Item);

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TArray<FSlot> Goods;

    UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite)
    int32 Money{100};

    virtual void BeginPlay() override;
    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
    UPROPERTY(Replicated)
    TArray<AActor*> Buyers;

    UPROPERTY(Replicated)
    TArray<FSlot> ShopSlots;

    uint8 ShopSlotsNumber = 12;

    virtual void Interact(AActor* InteractiveActor) override;
    bool FindStackOfSameItems(const FSlot& Item, uint8& OutSlotIndex, int32& OutAmount, bool& bOutCanStack);
    bool FindEmptySlot(uint8& OutSlotIndex);

    UFUNCTION(Server, unreliable)
    void InitShop_OnServer();
};
