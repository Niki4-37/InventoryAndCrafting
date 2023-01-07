// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/AwesomeShop.h"
#include "Net/UnrealNetwork.h"
#include "Components/InventoryComponent.h"

AAwesomeShop::AAwesomeShop()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
}

void AAwesomeShop::StopTrading(AActor* Buyer)
{
    Buyers.Remove(Buyer);
}

void AAwesomeShop::AddBuyer(AActor* Buyer)
{
    Buyers.Add(Buyer);
    UE_LOG(LogTemp, Display, TEXT("TotalBuyers: %i"), Buyers.Num());
}

void AAwesomeShop::BuyItem(uint8 Index)
{
    Goods.RemoveAt(Index);
    for (const auto& Buyer : Buyers)
    {
        if (!Buyer) continue;
        const auto InventoryComponent = Buyer->FindComponentByClass<UInventoryComponent>();
        if (!InventoryComponent) continue;
        InventoryComponent->UpdateShopWidgetAfterTransaction(Goods);
    }
}

bool AAwesomeShop::SellItem(const FSlot& Item)
{
    Goods.Add(Item);
    for (const auto& Buyer : Buyers)
    {
        if (!Buyer) continue;
        const auto InventoryComponent = Buyer->FindComponentByClass<UInventoryComponent>();
        if (!InventoryComponent) continue;
        InventoryComponent->UpdateShopWidgetAfterTransaction(Goods);
    }

    return true;
}

void AAwesomeShop::BeginPlay()
{
    Super::BeginPlay();
}

void AAwesomeShop::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AAwesomeShop, Goods);
    DOREPLIFETIME(AAwesomeShop, Buyers);
    DOREPLIFETIME(AAwesomeShop, Money);
}

void AAwesomeShop::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AAwesomeShop::Interact(AActor* InteractiveActor)
{
    if (!InteractiveActor) return;
    const auto InventoryComponent = InteractiveActor->FindComponentByClass<UInventoryComponent>();
    if (!InventoryComponent) return;
    InventoryComponent->StartTrading_OnServer(this);
}
