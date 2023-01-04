// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/AwesomeShop.h"
#include "Net/UnrealNetwork.h"
#include "Player/AwesomeBaseCharacter.h"

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
        const auto Player = Cast<AAwesomeBaseCharacter>(Buyer);
        if (!Player) continue;
        Player->UpdateShopWidgetAfterTransaction(Goods);
    }
}

bool AAwesomeShop::SellItem(const FSlot& Item)
{
    Goods.Add(Item);
    for (const auto& Buyer : Buyers)
    {
        const auto Player = Cast<AAwesomeBaseCharacter>(Buyer);
        if (!Player) continue;
        Player->UpdateShopWidgetAfterTransaction(Goods);
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
    const auto Player = Cast<AAwesomeBaseCharacter>(InteractiveActor);
    if (!Player) return;
    Player->StartTrading_OnServer(this);
}
