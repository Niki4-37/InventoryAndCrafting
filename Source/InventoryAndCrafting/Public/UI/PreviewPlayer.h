// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AwesomeTypes.h"
#include "PreviewPlayer.generated.h"

class USceneCaptureComponent2D;

UCLASS()
class INVENTORYANDCRAFTING_API APreviewPlayer : public AActor
{
    GENERATED_BODY()

public:
    APreviewPlayer();

    void CreateComponents();
    void SetEquipmentMesh(EEquipmentType Type, UStaticMesh* NewMesh);
    void SetSkeletalMeshAndAnimation(USkeletalMesh* NewMesh, UClass* AnimClass);

    UMaterialInstanceDynamic* CreateDynamicMaterialInstance();

protected:
    UPROPERTY()
    USceneComponent* ActorRootComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    USkeletalMeshComponent* SkeletalMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    USceneCaptureComponent2D* SceneCaptureComponent2D;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UMaterialInterface* RenderMaterial;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    FName RenderParameterName;

    virtual void BeginPlay() override;

private:
    UPROPERTY()
    TMap<EEquipmentType, UStaticMeshComponent*> EquipmentMeshesMap;
};
