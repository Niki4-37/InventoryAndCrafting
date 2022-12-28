// Fill out your copyright notice in the Description page of Project Settings.

#include "Pickup/AwesomePickupMaster.h"
#include "Player/AwesomeBaseCharacter.h"
#include "Net/UnrealNetwork.h"

AAwesomePickupMaster::AAwesomePickupMaster()
{
    bReplicates = true;
    GetStaticMeshComponent()->SetIsReplicated(true);
}

void AAwesomePickupMaster::BeginPlay()
{
    Super::BeginPlay();

    InitPickup();
}

void AAwesomePickupMaster::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AAwesomePickupMaster, PickupItem);
}

void AAwesomePickupMaster::Interact(AActor* InteractiveActor)
{
    const auto Player = Cast<AAwesomeBaseCharacter>(InteractiveActor);
    if (!Player) return;
    Player->PickupItem_OnServer(this);
}

void AAwesomePickupMaster::InitPickup()
{
    const auto PickupDataPointer = PickupItem.DataTableRowHandle.GetRow<FItemData>("");
    if (!PickupDataPointer) return;
    const auto PickupData = *PickupDataPointer;

    if (!GetStaticMeshComponent() || !PickupData.Mesh) return;
    GetStaticMeshComponent()->SetStaticMesh(PickupData.Mesh);
    GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
    GetStaticMeshComponent()->SetSimulatePhysics(true);
}
