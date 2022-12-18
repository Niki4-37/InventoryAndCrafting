// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/AwesomeBaseCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Pickup/AwesomeBackpackMaster.h"

#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY_STATIC(AwesomeCharacter, All, All);

AAwesomeBaseCharacter::AAwesomeBaseCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
    SpringArmComponent->SetupAttachment(GetRootComponent());
    SpringArmComponent->bUsePawnControlRotation = true;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
    CameraComponent->SetupAttachment(SpringArmComponent);
}

void AAwesomeBaseCharacter::EquipBackpack(AAwesomeBackpackMaster* Backpack)
{
    if (!Backpack) return;

    if (EquipedBackpack)
    {
        FDetachmentTransformRules DetachmentRules(EDetachmentRule::KeepWorld, false);
        EquipedBackpack->DetachFromActor(DetachmentRules);
        EquipedBackpack->GetStaticMeshComponent()->SetSimulatePhysics(true);
    }

    EquipedBackpack = Backpack;
    EquipedBackpack->GetStaticMeshComponent()->SetSimulatePhysics(false);
    FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
    EquipedBackpack->AttachToComponent(GetMesh(), AttachmentRules, BackpackSocketName);

    OnStuffEquiped.Broadcast();
}

TArray<FSlot> AAwesomeBaseCharacter::GetBackpackSlots() const
{
    if (EquipedBackpack)
    {
        return EquipedBackpack->GetBackpackSlots();
    }
    return TArray<FSlot>();
}

void AAwesomeBaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    InitEquipment();
}

void AAwesomeBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &AAwesomeBaseCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AAwesomeBaseCharacter::MoveRight);
    PlayerInputComponent->BindAxis("TurnAround", this, &AAwesomeBaseCharacter::AddControllerYawInput);
    PlayerInputComponent->BindAxis("LookUp", this, &AAwesomeBaseCharacter::AddControllerPitchInput);

    PlayerInputComponent->BindAction("TakeItem", IE_Pressed, this, &AAwesomeBaseCharacter::TakeItem);
}

bool AAwesomeBaseCharacter::FindStackOfSameItems(const FSlot& Item, uint8& OutSlotIndex, int32& OutAmount, bool& bOutCanStack)
{
    OutSlotIndex = -1;
    OutAmount = -1;
    bOutCanStack = false;
    if (!EquipmentSlots.Num() || !Item.DataTableRowHandle.DataTable) return false;

    const auto ItemDataPointer = Item.DataTableRowHandle.DataTable->FindRow<FItemData>(Item.DataTableRowHandle.RowName, "", false);
    if (!ItemDataPointer) return false;
    const auto ItemData = *ItemDataPointer;
    bOutCanStack = ItemData.bCanStack;
    if (!ItemData.bCanStack) return false;

    uint8 SlotIndex{0};
    for (const auto& SlotData : EquipmentSlots)
    {
        if (!SlotData.DataTableRowHandle.DataTable) continue;
        const auto SlotItemDataPointer = SlotData.DataTableRowHandle.DataTable->FindRow<FItemData>(SlotData.DataTableRowHandle.RowName, "", false);
        if (!SlotItemDataPointer) continue;
        const auto SlotItemData = *SlotItemDataPointer;
        if (SlotItemData.Name == ItemData.Name)
        {
            OutSlotIndex = SlotIndex;
            OutAmount = SlotData.Amount;
            return true;
        }
        ++SlotIndex;
    }

    return false;
}

bool AAwesomeBaseCharacter::FindEmptySlot(uint8& OutSlotIndex)
{
    OutSlotIndex = -1;
    if (!EquipmentSlots.Num()) return false;

    uint8 SlotIndex{0};
    for (const auto& SlotData : EquipmentSlots)
    {
        if (!SlotData.Amount)
        {
            OutSlotIndex = SlotIndex;
            return true;
        }
        ++SlotIndex;
    }
    return false;
}

bool AAwesomeBaseCharacter::RemoveAmountFromEquipmentSlotsAtIndex(const uint8 Index, const int32 AmountToRemove)
{
    return UpdateSlotItemData(Index, -AmountToRemove);
}

bool AAwesomeBaseCharacter::TryAddItemToSlots(const FSlot& Item)
{
    uint8 FoundSlotIndex;
    int32 FoundAmount;
    bool bCanStack;

    if (EquipedBackpack && EquipedBackpack->TryAddItemToSlots(Item))
    {
        return true;
    }

    if (FindStackOfSameItems(Item, FoundSlotIndex, FoundAmount, bCanStack))
    {
        if (bCanStack)
        {
            return UpdateSlotItemData(FoundSlotIndex, Item.Amount);
        }

        if (!bCanStack && FindEmptySlot(FoundSlotIndex))
        {
            EquipmentSlots[FoundSlotIndex] = Item;
            return UpdateSlotItemData(FoundSlotIndex, 0);
        }
        else
        {
            return false;
        }
    }
    else
    {
        if (FindEmptySlot(FoundSlotIndex))
        {
            EquipmentSlots[FoundSlotIndex] = Item;
            return UpdateSlotItemData(FoundSlotIndex, 0);
        }
        return false;
    }
}

bool AAwesomeBaseCharacter::TryAddItemToEquipmentSlotsByIndex(const FSlot& Item, const uint8 InIndex)
{
    if (!EquipmentSlots.IsValidIndex(InIndex)) return false;

    if (!EquipmentSlots[InIndex].Amount)
    {
        EquipmentSlots[InIndex] = Item;
        return UpdateSlotItemData(InIndex, 0);
    }

    if (Item.DataTableRowHandle.RowName == EquipmentSlots[InIndex].DataTableRowHandle.RowName)
    {
        const auto ItemDataPointer = Item.DataTableRowHandle.DataTable->FindRow<FItemData>(Item.DataTableRowHandle.RowName, "", false);
        if (!ItemDataPointer) return false;
        const auto ItemData = *ItemDataPointer;
        if (!ItemData.bCanStack) return false;

        return UpdateSlotItemData(InIndex, Item.Amount);
    }

    return false;
}

bool AAwesomeBaseCharacter::RemoveItemFromEquipmentSlots(const FSlot& Item)
{
    uint8 FoundSlotIndex;
    int32 FoundAmount;
    bool bCanStack;
    if (FindStackOfSameItems(Item, FoundSlotIndex, FoundAmount, bCanStack))
    {
        if (RemoveAmountFromEquipmentSlotsAtIndex(FoundSlotIndex, --FoundAmount)) return true;
    }

    return false;
}

void AAwesomeBaseCharacter::InitEquipment()
{
    for (uint8 Index = 0; Index < EquipmentSlotsNumber; ++Index)
    {
        EquipmentSlots.Add(FSlot());
    }
}

void AAwesomeBaseCharacter::MoveForward(float Amount)
{
    AddMovementInput(GetActorForwardVector(), Amount);
}

void AAwesomeBaseCharacter::MoveRight(float Amount)
{
    AddMovementInput(GetActorRightVector(), Amount);
}

void AAwesomeBaseCharacter::TakeItem()
{
    if (!GetWorld()) return;

    FVector ViewLocation;
    FRotator VeiwRotation;
    GetController()->GetPlayerViewPoint(ViewLocation, VeiwRotation);
    FVector Start = ViewLocation + (SpringArmComponent->TargetArmLength + 30.f) * VeiwRotation.Vector();
    FVector End = ViewLocation + (SpringArmComponent->TargetArmLength + 300.f) * VeiwRotation.Vector();

    FHitResult HitResult;
    UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, 20.f, ETraceTypeQuery::TraceTypeQuery1, false, {this}, EDrawDebugTrace::ForDuration, HitResult, true);
    // GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);

    if (HitResult.bBlockingHit)
    {
        DrawDebugLine(GetWorld(), Start, HitResult.ImpactPoint, FColor::Blue, false, 5.f, 0U, 3.f);
        if (const auto ActorWithInterface = Cast<IAwesomeInteractionInterface>(HitResult.GetActor()))
        {
            ActorWithInterface->Interact(this);
        }
    }
    else
    {
        DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 5.f, 0U, 3.f);
    }
}

bool AAwesomeBaseCharacter::UpdateSlotItemData(const uint8 Index, const int32 AmountModifier)
{
    if (!EquipmentSlots.IsValidIndex(Index)) return false;
    const auto Result = EquipmentSlots[Index].Amount + AmountModifier;
    if (Result > 99) return false;
    EquipmentSlots[Index].Amount = FMath::Clamp(Result, 0, 99);  // Set MAX in properties;
    EquipmentSlots[Index].ItemLocationType = EItemLocationType::Equipment;
    UE_LOG(AwesomeCharacter, Display, TEXT("%s added to slot: %i, count: %i"), *EquipmentSlots[Index].DataTableRowHandle.RowName.ToString(), Index, EquipmentSlots[Index].Amount)
    OnSlotsChanged.Broadcast(EquipmentSlots);
    return true;
}
