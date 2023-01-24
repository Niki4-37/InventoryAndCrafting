// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/PreviewPlayer.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/InventoryComponent.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/KismetMaterialLibrary.h"

APreviewPlayer::APreviewPlayer()
{
    PrimaryActorTick.bCanEverTick = false;
    ActorRootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");
    SetRootComponent(ActorRootComponent);

    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMesh");
    SkeletalMesh->SetupAttachment(GetRootComponent());

    SceneCaptureComponent2D = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCaptureComponent2D");
    SceneCaptureComponent2D->SetupAttachment(GetRootComponent());
    SceneCaptureComponent2D->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
}

void APreviewPlayer::CreateComponents()
{
    EquipmentMeshesMap.Empty();
    for (EEquipmentType EquipmentType = EEquipmentType::Begin; EquipmentType != EEquipmentType::End; ++EquipmentType)
    {
        FString EnumNameString(UEnum::GetValueAsName(EquipmentType).ToString());
        int32 ScopeIndex = EnumNameString.Find(TEXT("::"), ESearchCase::CaseSensitive);
        FName SocketName = NAME_None;
        if (ScopeIndex != INDEX_NONE)
        {
            SocketName = FName(*(EnumNameString.Mid(ScopeIndex + 2) + "Socket"));
        }
        auto Component = NewObject<UStaticMeshComponent>(this, SocketName);
        if (!Component) continue;
        Component->SetupAttachment(SkeletalMesh, SocketName);
        Component->RegisterComponent();
        EquipmentMeshesMap.Add(EquipmentType, Component);
    }
}

void APreviewPlayer::SetEquipmentMesh(EEquipmentType Type, UStaticMesh* NewMesh)
{
    for (const TPair<EEquipmentType, UStaticMeshComponent*>& Element : EquipmentMeshesMap)
    {
        UE_LOG(LogTemp, Display, TEXT("%s"), *Element.Value->GetName());
    }
    UE_LOG(LogTemp, Display, TEXT("%s"), *NewMesh->GetName());

    if (!EquipmentMeshesMap.Contains(Type)) return;
    EquipmentMeshesMap.FindRef(Type)->SetStaticMesh(NewMesh);
}

void APreviewPlayer::SetSkeletalMeshAndAnimation(USkeletalMesh* NewMesh, UClass* AnimClass)
{
    if (!NewMesh) return;
    SkeletalMesh->SetSkeletalMesh(NewMesh);
    SkeletalMesh->SetAnimInstanceClass(AnimClass);
}

UMaterialInstanceDynamic* APreviewPlayer::CreateDynamicMaterialInstance()
{
    if (!GetOwner() && !GetWorld()) return nullptr;
    SceneCaptureComponent2D->TextureTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld());
    FName MaterialName = FName(*(GetOwner()->GetName() + FString::FromInt(FMath::RandHelper(10000))));
    auto DynamicMaterial = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), RenderMaterial, MaterialName);
    if (!DynamicMaterial) return nullptr;
    DynamicMaterial->SetTextureParameterValue(RenderParameterName, SceneCaptureComponent2D->TextureTarget);
    return DynamicMaterial;
}

void APreviewPlayer::BeginPlay()
{
    Super::BeginPlay();

    SceneCaptureComponent2D->ShowOnlyActors.Add(this);
}
