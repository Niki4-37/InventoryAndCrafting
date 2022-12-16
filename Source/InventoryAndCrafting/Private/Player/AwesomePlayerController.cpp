// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/AwesomePlayerController.h"
#include "Pickup/AwesomePickupMaster.h"

void AAwesomePlayerController::SpawnDroppedItem(const FSlot& DroppedItem)
{
    UE_LOG(LogTemp, Display, TEXT("%s spawn in number: %i"), *DroppedItem.DataTableRowHandle.RowName.ToString(), DroppedItem.Amount)
    if (!GetWorld() || !GetPawn()) return;
    const auto SpawningLocation = GetPawn()->GetActorLocation() + GetPawn()->GetActorForwardVector() * 200.f;
    FTransform SpawnTransform(FRotator::ZeroRotator, SpawningLocation, FVector(1.f));
    UE_LOG(LogTemp, Display, TEXT("in location: %s"), *SpawningLocation.ToString());

    auto Pickup = GetWorld()->SpawnActorDeferred<AAwesomePickupMaster>(PickupMasterClass, SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
    if (Pickup)
    {
        Pickup->SetPickupItem(DroppedItem);
        Pickup->FinishSpawning(SpawnTransform);
    }
}