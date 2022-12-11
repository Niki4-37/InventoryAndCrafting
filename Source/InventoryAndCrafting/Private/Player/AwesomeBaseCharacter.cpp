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
    EquipedBackpack = Backpack;
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
    if (!Slots.Num() || !Item.DataTableRowHandle.DataTable) return false;

    const auto ItemDataPointer = Item.DataTableRowHandle.DataTable->FindRow<FInventoryData>(Item.DataTableRowHandle.RowName, "", false);
    if (!ItemDataPointer) return false;
    const auto ItemData = *ItemDataPointer;
    bOutCanStack = ItemData.bCanStack;

    uint8 SlotIndex{0};
    for (const auto& SlotData : Slots)
    {
        if (!SlotData.DataTableRowHandle.DataTable) continue;
        const auto SlotItemDataPointer = SlotData.DataTableRowHandle.DataTable->FindRow<FInventoryData>(SlotData.DataTableRowHandle.RowName, "", false);
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
    if (!Slots.Num()) return false;

    uint8 SlotIndex{0};
    for (const auto& SlotData : Slots)
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

bool AAwesomeBaseCharacter::RemoveAmountFromSlotsAtIndex(const uint8 Index, const int32 AmountToRemove)
{
    // if (!Slots.IsValidIndex(Index)) return false;
    // const auto Result = Slots[Index].Amount - AmountToRemove;
    // Slots[Index].Amount = FMath::Clamp(Result, 0, 999);  // Set MAX in properties;
    // OnSlotsChanged.Broadcast(Slots);
    // return true;
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
            Slots[FoundSlotIndex] = Item;
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
            Slots[FoundSlotIndex] = Item;
            return UpdateSlotItemData(FoundSlotIndex, 0);
        }
        return false;
    }
}

bool AAwesomeBaseCharacter::RemoveItemFromSlots(const FSlot& Item)
{
    uint8 FoundSlotIndex;
    int32 FoundAmount;
    bool bCanStack;
    if (FindStackOfSameItems(Item, FoundSlotIndex, FoundAmount, bCanStack))
    {
        if (RemoveAmountFromSlotsAtIndex(FoundSlotIndex, --FoundAmount)) return true;
    }

    return false;
}

void AAwesomeBaseCharacter::InitEquipment()
{
    for (uint8 Index = 0; Index < EquipmentSlots; ++Index)
    {
        Slots.Add(FSlot());
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
    FVector Start = ViewLocation + SpringArmComponent->TargetArmLength * VeiwRotation.Vector();
    FVector End = ViewLocation + (SpringArmComponent->TargetArmLength + 400.f) * VeiwRotation.Vector();

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
    if (!Slots.IsValidIndex(Index)) return false;
    const auto Result = Slots[Index].Amount + AmountModifier;
    Slots[Index].Amount = FMath::Clamp(Result, 0, 999);  // Set MAX in properties;
    UE_LOG(AwesomeCharacter, Display, TEXT("%s added to slot, count: %i"), *Slots[Index].DataTableRowHandle.RowName.ToString(), Slots[Index].Amount)
    OnSlotsChanged.Broadcast(Slots);
    return true;
}
