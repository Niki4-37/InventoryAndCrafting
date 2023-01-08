// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "EquipmentActor.generated.h"

/**
 *
 */
UCLASS()
class INVENTORYANDCRAFTING_API AEquipmentActor : public AStaticMeshActor
{
    GENERATED_BODY()

public:
    AEquipmentActor();

    void SetStaticMesh(class UStaticMesh* NewMesh);
};
