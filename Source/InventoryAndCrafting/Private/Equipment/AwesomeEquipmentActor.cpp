// Fill out your copyright notice in the Description page of Project Settings.

#include "Equipment/AwesomeEquipmentActor.h"

AAwesomeEquipmentActor::AAwesomeEquipmentActor()
{
    bReplicates = true;
    GetStaticMeshComponent()->SetIsReplicated(true);
    GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
    GetStaticMeshComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    GetStaticMeshComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
}

void AAwesomeEquipmentActor::SetStaticMesh(UStaticMesh* NewMesh)
{
    if (!NewMesh) return;
    GetStaticMeshComponent()->SetStaticMesh(NewMesh);
}