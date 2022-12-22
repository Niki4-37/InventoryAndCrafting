// Fill out your copyright notice in the Description page of Project Settings.

#include "Equipment/AwesomeVest.h"

AAwesomeVest::AAwesomeVest()
{
    GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
    GetStaticMeshComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    GetStaticMeshComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
}
