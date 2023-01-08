// Fill out your copyright notice in the Description page of Project Settings.

#include "Pickup/PickupMaster.h"
#include "Components/BoxComponent.h"
#include "Components/InventoryComponent.h"
#include "Net/UnrealNetwork.h"

APickupMaster::APickupMaster()
{
    bReplicates = true;
    GetStaticMeshComponent()->SetIsReplicated(true);

    BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
    BoxComponent->SetBoxExtent(FVector(50.f));
    BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    BoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
    BoxComponent->SetupAttachment(GetRootComponent());
}

void APickupMaster::BeginPlay()
{
    Super::BeginPlay();

    InitPickup();
}

void APickupMaster::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(APickupMaster, PickupItem);
}

void APickupMaster::Interact(AActor* InteractiveActor)
{
    if (!InteractiveActor) return;
    const auto InventoryComponent = InteractiveActor->FindComponentByClass<UInventoryComponent>();
    if (!InventoryComponent) return;
    InventoryComponent->PickupItem_OnServer(this);
}

void APickupMaster::InitPickup()
{
    const auto PickupDataPointer = PickupItem.DataTableRowHandle.GetRow<FItemData>("");
    if (!PickupDataPointer) return;
    const auto PickupData = *PickupDataPointer;

    if (!GetStaticMeshComponent() || !PickupData.Mesh) return;
    GetStaticMeshComponent()->SetStaticMesh(PickupData.Mesh);
    GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
    GetStaticMeshComponent()->SetSimulatePhysics(true);
}
