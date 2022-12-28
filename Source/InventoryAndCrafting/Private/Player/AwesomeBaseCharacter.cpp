// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/AwesomeBaseCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Pickup/AwesomeBackpackMaster.h"
#include "Pickup/AwesomePickupMaster.h"
#include "Player/AwesomePlayerController.h"
#include "Equipment/AwesomeEquipmentActor.h"
#include "Net/UnrealNetwork.h"

#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY_STATIC(AwesomeCharacter, All, All);

AAwesomeBaseCharacter::AAwesomeBaseCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
    SpringArmComponent->SetupAttachment(GetRootComponent());
    SpringArmComponent->bUsePawnControlRotation = true;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
    CameraComponent->SetupAttachment(SpringArmComponent);
}

void AAwesomeBaseCharacter::EquipBackpack_OnServer_Implementation(AAwesomeBackpackMaster* Backpack)
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

    EquipedBackpack->SetOwner(this);

    OnStuffEquiped_OnClient(EquipedBackpack->GetBackpackSlots(), ESlotLocationType::Inventory);
}

void AAwesomeBaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        InitEnableSlots_OnServer();
    }

    OnStuffEquiped.Broadcast(PersonalSlots, ESlotLocationType::PersonalSlots);
}

void AAwesomeBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AAwesomeBaseCharacter, EquipedBackpack);
    DOREPLIFETIME(AAwesomeBaseCharacter, PersonalSlotsNumber);
    DOREPLIFETIME(AAwesomeBaseCharacter, PersonalSlots);
    DOREPLIFETIME(AAwesomeBaseCharacter, EquipmentSlots);
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

void AAwesomeBaseCharacter::UpdateInventoryWidgetSlotData(const FSlot& Item, const uint8 Index)
{
    OnSlotChanged_OnClient(Item, Index, ESlotLocationType::Inventory);
}

void AAwesomeBaseCharacter::PickupItem_OnServer_Implementation(AAwesomePickupMaster* Pickup)
{
    if (!Pickup) return;
    if (TryAddItemToSlots(Pickup->GetPickupItem()))
    {
        Pickup->Destroy();
    }
}

bool AAwesomeBaseCharacter::TryAddItemToSlots(const FSlot& Item)
{
    /* handle on server */
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

void AAwesomeBaseCharacter::MoveItem_OnServer_Implementation(const FSlot& Item,                   //
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
            if (!GetBackpack()) return;
            bMoveSuccess = GetBackpack()->TryAddItemToSlots(Item);
            break;
        }
        case (ESlotLocationType::PersonalSlots): bMoveSuccess = TryAddItemToPersonalSlotsByIndex(Item, ToSlotIndex); break;
        case (ESlotLocationType::Equipment): bMoveSuccess = TryAddItemToEquipment(Item, ToEquipmentType); break;
        case (ESlotLocationType::Environment):
        {
            const auto AwesomePlayerController = Cast<AAwesomePlayerController>(Controller);
            if (!AwesomePlayerController) return;
            AwesomePlayerController->SpawnDroppedItem(Item);
            bMoveSuccess = true;
            break;
        }

        default: bMoveSuccess = false;
    }

    if (!bMoveSuccess) return;

    switch (FromLocationType)
    {
        case (ESlotLocationType::Inventory):
        {
            if (!GetBackpack()) return;
            GetBackpack()->RemoveAmountFromInventorySlotsAtIndex(FromSlotIndex, Item.Amount);
            break;
        }
        case (ESlotLocationType::PersonalSlots): RemoveAmountFromChoosenSlotsAtIndex(PersonalSlots, FromSlotIndex, Item.Amount); break;
        case (ESlotLocationType::Equipment): RemoveItemFromEquipment(Item, FromEquipmentType); break;
    }
}

void AAwesomeBaseCharacter::InitEnableSlots_OnServer_Implementation()
{
    for (uint8 Index = 0; Index < PersonalSlotsNumber; ++Index)
    {
        PersonalSlots.Add(FSlot());
    }

    for (EEquipmentType EquipmentType = EEquipmentType::Begin; EquipmentType != EEquipmentType::End; ++EquipmentType)
    {
        EquipmentSlots.Add(FEquipmentSlot(EquipmentType, FDataTableRowHandle(), 0));
        UE_LOG(AwesomeCharacter, Display, TEXT("%s"), *UEnum::GetValueAsString(EquipmentType));
    }
    FVector;
}

bool AAwesomeBaseCharacter::TryAddItemToPersonalSlotsByIndex(const FSlot& Item, const uint8 InIndex)
{
    /* handle on server */
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
    /* handle on server */
    const auto ItemDataPointer = Item.DataTableRowHandle.GetRow<FItemData>("");
    if (!ItemDataPointer) return false;
    const auto ItemData = *ItemDataPointer;
    auto ItemEquipmentType = ItemData.EquipmnetType;
    if (ItemData.EquipmnetType == EEquipmentType::NotEquipment) return false;

    CheckForWeapon(ItemEquipmentType, ToEquipmentType);

    auto FoundSlotPtr = EquipmentSlots.FindByPredicate([&](FEquipmentSlot& Data) { return Data.EquipmentType == ToEquipmentType; });
    if (!FoundSlotPtr) return false;

    auto FoundSlot = *FoundSlotPtr;

    if (FoundSlot.Amount)
    {
        UE_LOG(AwesomeCharacter, Display, TEXT("Slot occupied"));
        if (TryAddItemToSlots(FSlot(FoundSlot.DataTableRowHandle, FoundSlot.Amount)))
        {
            RemoveItemFromEquipment(FSlot(FoundSlot.DataTableRowHandle, FoundSlot.Amount), ItemEquipmentType);
        }
        else
            return false;
    }
    *FoundSlotPtr = FEquipmentSlot(ItemEquipmentType, Item.DataTableRowHandle, Item.Amount);

    OnEquipmentSlotDataChanged_OnClient(Item, ItemEquipmentType);
    const auto SocketName = EquipmentSocketNamesMap.FindChecked(ItemEquipmentType);
    EquipItem(ItemData.ActorClass, ItemData.Mesh, SocketName, ItemEquipmentType);

    for (const auto& Element : EquipmentSlots)
    {
        UE_LOG(AwesomeCharacter, Display, TEXT("%s equiped %s amount %i"), *UEnum::GetValueAsString(Element.EquipmentType), *Element.DataTableRowHandle.RowName.ToString(), Element.Amount);
    }
    return true;
}

bool AAwesomeBaseCharacter::CheckForWeapon(EEquipmentType& WeaponEquipmentType, EEquipmentType ToEquipmentType)
{
    /* handle on server */
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
    /* handle on server */
    return UpdateSlotItemData(Slots, Index, -AmountToRemove);
}

bool AAwesomeBaseCharacter::RemoveItemFromPersonalSlots(const FSlot& Item)
{
    /* handle on server */
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
    /* handle on server */
    auto FoundSlotPtr = EquipmentSlots.FindByPredicate([&](FEquipmentSlot& Data) { return Data.EquipmentType == FromEquipmentType; });
    if (!FoundSlotPtr) return false;
    auto FoundSlot = *FoundSlotPtr;
    *FoundSlotPtr = FEquipmentSlot(FromEquipmentType, FDataTableRowHandle(), 0);

    OnEquipmentSlotDataChanged_OnClient(FSlot(), FromEquipmentType);
    if (auto EquippedItem = EquippedItemsMap.FindAndRemoveChecked(FromEquipmentType))
    {
        EquippedItem->Destroy();
    }

    for (const auto& Element : EquipmentSlots)
    {
        UE_LOG(AwesomeCharacter, Display, TEXT("%s equiped %s amount %i"), *UEnum::GetValueAsString(Element.EquipmentType), *Element.DataTableRowHandle.RowName.ToString(), Element.Amount);
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
    /* handle on server */
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
    /* handle on server */
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
    /* handle on server */
    if (!Slots.IsValidIndex(Index)) return false;
    const auto Result = Slots[Index].Amount + AmountModifier;
    if (Result > 99) return false;
    Slots[Index].Amount = FMath::Clamp(Result, 0, 99);  // Set MAX in properties;
    if (!Slots[Index].Amount)
    {
        Slots[Index] = FSlot();
    }
    UE_LOG(AwesomeCharacter, Display, TEXT("%s added to slot: %i, count: %i"), *Slots[Index].DataTableRowHandle.RowName.ToString(), Index, Slots[Index].Amount);
    OnSlotChanged_OnClient(Slots[Index], Index, ESlotLocationType::PersonalSlots);
    return true;
}

void AAwesomeBaseCharacter::EquipItem(UClass* Class, UStaticMesh* NewMesh, FName SocketName, EEquipmentType Type)
{
    /* handle on server */
    if (!GetWorld()) return;
    auto PlayerEquippedItem = GetWorld()->SpawnActor<AAwesomeEquipmentActor>(Class);
    if (!PlayerEquippedItem) return;
    // PlayerEquippedItem->SetStaticMesh(NewMesh);
    SetStaticMesh_Multicast(PlayerEquippedItem, NewMesh);
    FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
    PlayerEquippedItem->AttachToComponent(GetMesh(), AttachmentRules, SocketName);
    EquippedItemsMap.Add(Type, PlayerEquippedItem);
}

void AAwesomeBaseCharacter::OnStuffEquiped_OnClient_Implementation(const TArray<FSlot>& Slots, ESlotLocationType Type)
{
    if (Controller && Controller->IsLocalPlayerController())
    {
        OnStuffEquiped.Broadcast(Slots, Type);
    }
}

void AAwesomeBaseCharacter::OnSlotChanged_OnClient_Implementation(const FSlot& Item, const uint8 Index, ESlotLocationType Type)
{
    OnSlotChanged.Broadcast(Item, Index, Type);
}

void AAwesomeBaseCharacter::OnEquipmentSlotDataChanged_OnClient_Implementation(const FSlot& Item, EEquipmentType Type)
{
    OnEquipmentSlotDataChanged.Broadcast(Item, Type);
}

void AAwesomeBaseCharacter::SetStaticMesh_Multicast_Implementation(AAwesomeEquipmentActor* Actor, UStaticMesh* NewMesh)
{
    if (!Actor || !NewMesh) return;
    Actor->SetStaticMesh(NewMesh);
}