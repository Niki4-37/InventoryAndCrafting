// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AwesomeTypes.h"
#include "Interfaces/AwesomeInteractionInterface.h"
#include "AwesomeShop.generated.h"

UCLASS()
class INVENTORYANDCRAFTING_API AAwesomeShop : public ACharacter, public IAwesomeInteractionInterface
{
    GENERATED_BODY()

public:
    AAwesomeShop();

    TArray<FSlot> GetGoods() const { return Goods; };

    void AddBuyer(AActor* Buyer);
    void StopTrading(AActor* Buyer);
    void BuyItem(uint8 Index);
    bool SellItem(const FSlot& Item);

protected:
    UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite)
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

    virtual void Interact(AActor* InteractiveActor) override;
};
