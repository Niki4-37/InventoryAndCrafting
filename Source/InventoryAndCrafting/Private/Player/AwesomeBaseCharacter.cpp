// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/AwesomeBaseCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Pickup/AwesomeBackpackMaster.h"
#include "Player/AwesomePlayerController.h"
#include "Equipment/AwesomeEquipmentActor.h"

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

    OnStuffEquiped.Broadcast(EquipedBackpack->GetBackpackSlots(), ESlotLocationType::Inventory);
}

void AAwesomeBaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    InitPersonalSlots();
    InitEquipmentSlots();
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
            return UpdateSlotItemData(PersonalSlots, FoundSlotIndex, Item.Amount);
        }

        if (!bCanStack && FindEmptySlot(FoundSlotIndex))
        {
            PersonalSlots[FoundSlotIndex] = Item;
            return UpdateSlotItemData(PersonalSlots, FoundSlotIndex, 0);
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
            PersonalSlots[FoundSlotIndex] = Item;
            return UpdateSlotItemData(PersonalSlots, FoundSlotIndex, 0);
        }
        return false;
    }
}

bool AAwesomeBaseCharacter::MoveItem(const FSlot& Item,                   //
                                     ESlotLocationType FromLocationType,  //
                                     EEquipmentType FromEquipmentType,    //
                                     const uint8 FromSlotIndex,           //
                                     ESlotLocationType ToLocationType,    //
                                     EEquipmentType ToEquipmentType,      //
                                     const uint8 ToSlotIndex)
{
    UE_LOG(AwesomeCharacter, Display, TEXT("MoveItem from %s slot %i to %s slot %i amount: %i"), *UEnum::GetValueAsString(FromLocationType), FromSlotIndex, *UEnum::GetValueAsString(ToLocationType),
           ToSlotIndex, Item.Amount);
    bool bMoveSuccess{false};
    switch (ToLocationType)
    {
        case (ESlotLocationType::Inventory):
        {
            if (!GetBackpack()) return false;
            bMoveSuccess = GetBackpack()->TryAddItemToSlots(Item);
            break;
        }
        case (ESlotLocationType::PersonalSlots): bMoveSuccess = TryAddItemToPersonalSlotsByIndex(Item, ToSlotIndex); break;
        case (ESlotLocationType::Equipment): bMoveSuccess = TryAddItemToEquipment(Item, ToEquipmentType); break;
        case (ESlotLocationType::Environment):
        {
            const auto AwesomePlayerController = Cast<AAwesomePlayerController>(Controller);
            if (!AwesomePlayerController) return false;
            AwesomePlayerController->SpawnDroppedItem(Item);
            bMoveSuccess = true;
            break;
        }

        default: bMoveSuccess = false;
    }

    if (!bMoveSuccess) return false;

    switch (FromLocationType)
    {
        case (ESlotLocationType::Inventory):
        {
            if (!GetBackpack()) return false;
            GetBackpack()->RemoveAmountFromInventorySlotsAtIndex(FromSlotIndex, Item.Amount);
            break;
        }
        case (ESlotLocationType::PersonalSlots): RemoveAmountFromChoosenSlotsAtIndex(PersonalSlots, FromSlotIndex, Item.Amount); break;
        case (ESlotLocationType::Equipment): RemoveItemFromEquipment(Item, FromEquipmentType); break;
    }

    return bMoveSuccess;
}

void AAwesomeBaseCharacter::InitPersonalSlots()
{
    for (uint8 Index = 0; Index < PersonalSlotsNumber; ++Index)
    {
        PersonalSlots.Add(FSlot());
    }
    OnStuffEquiped.Broadcast(PersonalSlots, ESlotLocationType::PersonalSlots);
}

void AAwesomeBaseCharacter::InitEquipmentSlots()
{
    for (EEquipmentType EquipmentSlot = EEquipmentType::Begin; EquipmentSlot != EEquipmentType::End; ++EquipmentSlot)
    {
        EquipmentSlotsMap.Add(EquipmentSlot, FSlot());
        // UE_LOG(AwesomeCharacter, Display, TEXT("%s"), *UEnum::GetValueAsString(EquipmentSlot));
    }
}

bool AAwesomeBaseCharacter::TryAddItemToPersonalSlotsByIndex(const FSlot& Item, const uint8 InIndex)
{
    if (!PersonalSlots.IsValidIndex(InIndex)) return false;

    if (!PersonalSlots[InIndex].Amount)
    {
        PersonalSlots[InIndex] = Item;
        return UpdateSlotItemData(PersonalSlots, InIndex, 0);
    }

    if (Item.DataTableRowHandle.RowName == PersonalSlots[InIndex].DataTableRowHandle.RowName)
    {
        const auto ItemDataPointer = Item.DataTableRowHandle.GetRow<FItemData>("");
        if (!ItemDataPointer) return false;
        const auto ItemData = *ItemDataPointer;
        if (!ItemData.bCanStack) return false;

        return UpdateSlotItemData(PersonalSlots, InIndex, Item.Amount);
    }

    return false;
}

bool AAwesomeBaseCharacter::TryAddItemToEquipment(const FSlot& Item, EEquipmentType ToEquipmentType)
{
    const auto ItemDataPointer = Item.DataTableRowHandle.GetRow<FItemData>("");
    if (!ItemDataPointer) return false;
    const auto ItemData = *ItemDataPointer;
    auto ItemEquipmentType = ItemData.EquipmnetType;

    CheckForWeapon(ItemEquipmentType, ToEquipmentType);

    auto FoundSlot = EquipmentSlotsMap.FindRef(ItemEquipmentType);
    if (FoundSlot.Amount)
    {
        UE_LOG(AwesomeCharacter, Display, TEXT("Slot occupied"));
        if (TryAddItemToSlots(FoundSlot))
        {
            RemoveItemFromEquipment(FoundSlot, ItemEquipmentType);
        }
        else
            return false;
    }
    EquipmentSlotsMap.Emplace(ItemEquipmentType, Item);
    OnEquipmentSlotDataChanged.Broadcast(Item, ItemEquipmentType);
    const auto SocketName = EquipmentSocketNamesMap.FindChecked(ItemEquipmentType);
    EquipItem(ItemData.ActorClass, ItemData.Mesh, SocketName, ItemEquipmentType);

    for (TPair<EEquipmentType, FSlot>& Element : EquipmentSlotsMap)
    {
        UE_LOG(AwesomeCharacter, Display, TEXT("%s equiped %s amount %i"), *UEnum::GetValueAsString(Element.Key), *Element.Value.DataTableRowHandle.RowName.ToString(), Element.Value.Amount);
    }
    return true;
}

bool AAwesomeBaseCharacter::CheckForWeapon(EEquipmentType& WeaponEquipmentType, EEquipmentType ToEquipmentType)
{
    if (WeaponEquipmentType == EEquipmentType::RightArm && ToEquipmentType == EEquipmentType::RightArm) return true;

    if (WeaponEquipmentType == EEquipmentType::RightArm && ToEquipmentType == EEquipmentType::LeftArm)
    {
        WeaponEquipmentType = ToEquipmentType;
        return true;
    }

    return false;
}

bool AAwesomeBaseCharacter::RemoveAmountFromChoosenSlotsAtIndex(TArray<FSlot>& Slots, const uint8 Index, const int32 AmountToRemove)
{
    return UpdateSlotItemData(Slots, Index, -AmountToRemove);
}

bool AAwesomeBaseCharacter::RemoveItemFromPersonalSlots(const FSlot& Item)
{
    uint8 FoundSlotIndex;
    int32 FoundAmount;
    bool bCanStack;
    if (FindStackOfSameItems(Item, FoundSlotIndex, FoundAmount, bCanStack))
    {
        if (RemoveAmountFromChoosenSlotsAtIndex(PersonalSlots, FoundSlotIndex, --FoundAmount)) return true;
    }

    return false;
}

bool AAwesomeBaseCharacter::RemoveItemFromEquipment(const FSlot& Item, EEquipmentType FromEquipmentType)
{
    // const auto KeyPionter = EquipmentSlotsMap.FindKey(Item);
    // if (!KeyPionter) return false;
    // auto FoundKey = *KeyPionter;

    EquipmentSlotsMap.Emplace(FromEquipmentType, FSlot());
    OnEquipmentSlotDataChanged.Broadcast(FSlot(), FromEquipmentType);
    if (auto EquippedItem = EquippedItemsMap.FindAndRemoveChecked(FromEquipmentType))
    {
        EquippedItem->Destroy();
    }

    for (TPair<EEquipmentType, FSlot>& Element : EquipmentSlotsMap)
    {
        UE_LOG(AwesomeCharacter, Display, TEXT("%s equiped %s amount %i"), *UEnum::GetValueAsString(Element.Key), *Element.Value.DataTableRowHandle.RowName.ToString(), Element.Value.Amount);
    }
    return false;
}

void AAwesomeBaseCharacter::MoveForward(float Amount)
{
    AddMovementInput(GetActorForwardVector(), Amount);
}

void AAwesomeBaseCharacter::MoveRight(float Amount)
{
    AddMovementInput(GetActorRightVector(), Amount);
}

bool AAwesomeBaseCharacter::FindStackOfSameItems(const FSlot& Item, uint8& OutSlotIndex, int32& OutAmount, bool& bOutCanStack)
{
    OutSlotIndex = -1;
    OutAmount = -1;
    bOutCanStack = false;
    if (!PersonalSlots.Num()) return false;

    const auto ItemDataPointer = Item.DataTableRowHandle.GetRow<FItemData>("");
    if (!ItemDataPointer) return false;
    const auto ItemData = *ItemDataPointer;
    bOutCanStack = ItemData.bCanStack;
    if (!ItemData.bCanStack) return false;

    uint8 SlotIndex{0};
    for (const auto& SlotData : PersonalSlots)
    {
        const auto SlotItemDataPointer = SlotData.DataTableRowHandle.GetRow<FItemData>("");
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
    if (!PersonalSlots.Num()) return false;

    uint8 SlotIndex{0};
    for (const auto& SlotData : PersonalSlots)
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

bool AAwesomeBaseCharacter::UpdateSlotItemData(TArray<FSlot>& Slots, const uint8 Index, const int32 AmountModifier)
{
    if (!Slots.IsValidIndex(Index)) return false;
    const auto Result = Slots[Index].Amount + AmountModifier;
    if (Result > 99) return false;
    Slots[Index].Amount = FMath::Clamp(Result, 0, 99);  // Set MAX in properties;
    if (!Slots[Index].Amount)
    {
        Slots[Index] = FSlot();
    }
    UE_LOG(AwesomeCharacter, Display, TEXT("%s added to slot: %i, count: %i"), *Slots[Index].DataTableRowHandle.RowName.ToString(), Index, Slots[Index].Amount);
    OnSlotChanged.Broadcast(Slots[Index], Index);
    return true;
}

void AAwesomeBaseCharacter::EquipItem(UClass* Class, UStaticMesh* NewMesh, FName SocketName, EEquipmentType Type)
{
    if (!GetWorld()) return;
    auto PlayerEquippedItem = GetWorld()->SpawnActor<AAwesomeEquipmentActor>(Class);
    if (!PlayerEquippedItem) return;
    PlayerEquippedItem->SetStaticMesh(NewMesh);
    FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
    PlayerEquippedItem->AttachToComponent(GetMesh(), AttachmentRules, SocketName);
    EquippedItemsMap.Add(Type, PlayerEquippedItem);
}
