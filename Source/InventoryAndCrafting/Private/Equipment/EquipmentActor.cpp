// Fill out your copyright notice in the Description page of Project Settings.

#include "Equipment/EquipmentActor.h"

AEquipmentActor::AEquipmentActor()
{
    bReplicates = true;
    GetStaticMeshComponent()->SetIsReplicated(true);
    GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
    GetStaticMeshComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    GetStaticMeshComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
}

void AEquipmentActor::SetStaticMesh(UStaticMesh* NewMesh)
{
    if (!NewMesh) return;
    GetStaticMeshComponent()->SetStaticMesh(NewMesh);
}