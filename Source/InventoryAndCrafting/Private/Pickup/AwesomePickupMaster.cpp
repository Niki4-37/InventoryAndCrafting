// Fill out your copyright notice in the Description page of Project Settings.

#include "Pickup/AwesomePickupMaster.h"
#include "Player/AwesomeBaseCharacter.h"

AAwesomePickupMaster::AAwesomePickupMaster()
{
    // InitPickup();
}

void AAwesomePickupMaster::BeginPlay()
{
    Super::BeginPlay();

    InitPickup();

    GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
    GetStaticMeshComponent()->SetSimulatePhysics(true);
}

void AAwesomePickupMaster::Interact(AActor* InteractiveActor)
{
    const auto Player = Cast<AAwesomeBaseCharacter>(InteractiveActor);
    if (!Player) return;
    if (Player->TryAddItemToSlots(PickupItem))
    {
        Destroy();
    }
}

void AAwesomePickupMaster::InitPickup()
{
    const auto PickupDataPointer = PickupItem.DataTableRowHandle.GetRow<FItemData>("");
    if (!PickupDataPointer) return;
    const auto PickupData = *PickupDataPointer;

    if (!GetStaticMeshComponent() || !PickupData.Mesh) return;
    GetStaticMeshComponent()->SetStaticMesh(PickupData.Mesh);
}
