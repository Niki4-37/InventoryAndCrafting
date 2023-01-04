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
#include "AI/AwesomeShop.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"

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

void AAwesomeBaseCharacter::StartTrading_OnServer_Implementation(AAwesomeShop* Shop)
{
    if (!Shop || ActiveShop) return;
    ActiveShop = Shop;
    ActiveShop->AddBuyer(this);
    OnStuffEquiped_OnClient(Shop->GetGoods(), ESlotLocationType::ShopSlots);
    OnTrading_OnClient(true);
    OpenInventory_OnClient();
}

void AAwesomeBaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        Money = 100;
        InitEnableSlots_OnServer();
    }
    OnMoneyChanged_OnClient(Money);
    OnStuffEquiped.Broadcast(PersonalSlots, ESlotLocationType::PersonalSlots);

    const auto AwesomeController = Cast<AAwesomePlayerController>(Controller);
    if (AwesomeController)
    {
        AwesomeController->OnHUDWidgetSwitch.AddUObject(this, &AAwesomeBaseCharacter::OnHUDWidgetSwitch);
    }
}

void AAwesomeBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AAwesomeBaseCharacter, EquipedBackpack);
    DOREPLIFETIME(AAwesomeBaseCharacter, PersonalSlotsNumber);
    DOREPLIFETIME(AAwesomeBaseCharacter, PersonalSlots);
    DOREPLIFETIME(AAwesomeBaseCharacter, EquipmentSlots);
    DOREPLIFETIME(AAwesomeBaseCharacter, ActiveShop);
    DOREPLIFETIME(AAwesomeBaseCharacter, Money);
}

void AAwesomeBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &AAwesomeBaseCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AAwesomeBaseCharacter::MoveRight);
    PlayerInputComponent->BindAxis("TurnAround", this, &AAwesomeBaseCharacter::AddControllerYawInput);
    PlayerInputComponent->BindAxis("LookUp", this, &AAwesomeBaseCharacter::AddControllerPitchInput);

    PlayerInputComponent->BindAction("TakeItem", IE_Pressed, this, &AAwesomeBaseCharacter::TakeItem);
    PlayerInputComponent->BindAction("SwapWeapons", IE_Pressed, this, &AAwesomeBaseCharacter::SwapWeapons_OnServer);
    PlayerInputComponent->BindAction("DrawWeapon", IE_Pressed, this, &AAwesomeBaseCharacter::DrawWeapon_OnServer);
}

void AAwesomeBaseCharacter::UpdateWidgetSlotData(const FSlot& Item, const uint8 Index, ESlotLocationType Type)
{
    OnSlotChanged_OnClient(Item, Index, Type);
}

void AAwesomeBaseCharacter::UpdateShopWidgetAfterTransaction(const TArray<FSlot>& Goods)
{
    OnStuffEquiped_OnClient(Goods, ESlotLocationType::ShopSlots);
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
        case (ESlotLocationType::ShopSlots): bMoveSuccess = TrySellItem(Item); break;

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
        case (ESlotLocationType::Equipment): RemoveItemFromEquipment(FromEquipmentType); break;
        case (ESlotLocationType::ShopSlots): BuyItem(Item, FromSlotIndex);
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
    if (!Item.Amount) return false;
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
            RemoveItemFromEquipment(ItemEquipmentType);
        }
        else
            return false;
    }
    *FoundSlotPtr = FEquipmentSlot(ItemEquipmentType, Item.DataTableRowHandle, Item.Amount);

    if (ItemData.bCanIncreasePersonalSlots)
    {
        PersonalSlots.Append(ItemData.PresonalExtraSlots);
        OnStuffEquiped_OnClient(PersonalSlots, ESlotLocationType::PersonalSlots);
    }

    OnEquipmentSlotDataChanged_OnClient(Item, ItemEquipmentType);
    const auto SocketName = EquipmentSocketNamesMap.FindChecked(ItemEquipmentType);
    EquipItem(ItemData.ActorClass, ItemData.Mesh, SocketName, ItemEquipmentType);

    return true;
}

bool AAwesomeBaseCharacter::CheckForWeapon(EEquipmentType& WeaponEquipmentType, EEquipmentType ToEquipmentType)
{
    switch (ToEquipmentType)
    {
        case (EEquipmentType::RightArm):
        {
            if (WeaponEquipmentType == EEquipmentType::RightArm) return true;
        }
        case (EEquipmentType::LeftArm):
        {
            if (WeaponEquipmentType == EEquipmentType::RightArm)
            {
                WeaponEquipmentType = ToEquipmentType;
                return true;
            }
        }
        case (EEquipmentType::Back):
        {
            if (WeaponEquipmentType == EEquipmentType::RightArm)
            {
                WeaponEquipmentType = ToEquipmentType;
                return true;
            }
            if (WeaponEquipmentType == EEquipmentType::LeftArm)
            {
                WeaponEquipmentType = ToEquipmentType;
                return true;
            }
        }
        default: return false;
    }
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

FSlot AAwesomeBaseCharacter::RemoveItemFromEquipment(EEquipmentType FromEquipmentType)
{
    /* handle on server */
    auto FoundSlotPtr = EquipmentSlots.FindByPredicate([&](FEquipmentSlot& Data) { return Data.EquipmentType == FromEquipmentType; });
    if (!FoundSlotPtr || !FoundSlotPtr->Amount) return FSlot();
    auto FoundSlot = *FoundSlotPtr;
    *FoundSlotPtr = FEquipmentSlot(FromEquipmentType, FDataTableRowHandle(), 0);

    if (auto FoundRow = FoundSlot.DataTableRowHandle.GetRow<FItemData>(""))
    {
        RemovePersonalExtraSlots(*FoundRow);
    }

    OnEquipmentSlotDataChanged_OnClient(FSlot(), FromEquipmentType);
    if (auto EquippedItem = EquippedItemsMap.FindAndRemoveChecked(FromEquipmentType))
    {
        EquippedItem->Destroy();
    }

    return FSlot(FoundSlot.DataTableRowHandle, FoundSlot.Amount);
}

void AAwesomeBaseCharacter::RemovePersonalExtraSlots(const FItemData& ItemData)
{
    if (!ItemData.bCanIncreasePersonalSlots) return;
    const auto AwesomePlayerController = Cast<AAwesomePlayerController>(Controller);
    auto ExtraSlotsNumber = ItemData.PresonalExtraSlots.Num();
    while (ExtraSlotsNumber)
    {
        if (AwesomePlayerController)
        {
            AwesomePlayerController->SpawnDroppedItem(PersonalSlots.Last());
        }
        else
        {
            UE_LOG(AwesomeCharacter, Warning, TEXT("Can't find AwesomePlayerController"));
        }
        PersonalSlots.Pop();
        --ExtraSlotsNumber;
    }
    OnStuffEquiped_OnClient(PersonalSlots, ESlotLocationType::PersonalSlots);
}

bool AAwesomeBaseCharacter::HasEquipmentToSwap(EEquipmentType FirstSlotType, EEquipmentType SecondSlotType)
{
    const auto FirstSlotPtr = EquipmentSlots.FindByPredicate([&](FEquipmentSlot& Data) { return Data.EquipmentType == FirstSlotType; });
    const auto SecondSlotPtr = EquipmentSlots.FindByPredicate([&](FEquipmentSlot& Data) { return Data.EquipmentType == SecondSlotType; });

    if (!FirstSlotPtr || !SecondSlotPtr) return false;

    return FirstSlotPtr->Amount || SecondSlotPtr->Amount;
}

void AAwesomeBaseCharacter::SwapWeapons_OnServer_Implementation()
{
    if (!HasEquipmentToSwap(EEquipmentType::RightArm, EEquipmentType::LeftArm)  //
        || bIsPlayingMontage                                                    //
        || !GetWorld())
        return;
    bIsPlayingMontage = true;
    float MontagePlayRate = 2.f;
    float HalfDuration = SwapWeaponsMontage ? SwapWeaponsMontage->CalculateSequenceLength() / (2 * MontagePlayRate) : 0.f;
    PlayAnimMontage_Multicast(SwapWeaponsMontage, MontagePlayRate);

    GetWorld()->GetTimerManager().SetTimer(PlayerActionTimer, this, &AAwesomeBaseCharacter::SwapWeapons, HalfDuration);
}

void AAwesomeBaseCharacter::DrawWeapon_OnServer_Implementation()
{
    if (!HasEquipmentToSwap(EEquipmentType::RightArm, EEquipmentType::Back)  //
        || bIsPlayingMontage                                                 //
        || !GetWorld())
        return;
    bIsPlayingMontage = true;
    float MontagePlayRate = 2.f;
    float HalfDuration = DrawWeaponMontage ? DrawWeaponMontage->CalculateSequenceLength() / (2 * MontagePlayRate) : 0.f;
    PlayAnimMontage_Multicast(DrawWeaponMontage, MontagePlayRate);

    GetWorld()->GetTimerManager().SetTimer(PlayerActionTimer, this, &AAwesomeBaseCharacter::DrawWeapon, HalfDuration);
}

void AAwesomeBaseCharacter::SwapWeapons()
{
    const auto ItemFromRightArm = RemoveItemFromEquipment(EEquipmentType::RightArm);
    const auto ItemFromLeftArm = RemoveItemFromEquipment(EEquipmentType::LeftArm);

    TryAddItemToEquipment(ItemFromRightArm, EEquipmentType::LeftArm);
    TryAddItemToEquipment(ItemFromLeftArm, EEquipmentType::RightArm);

    bIsPlayingMontage = false;
    GetWorld()->GetTimerManager().ClearTimer(PlayerActionTimer);
}

void AAwesomeBaseCharacter::DrawWeapon()
{
    const auto ItemFromRightArm = RemoveItemFromEquipment(EEquipmentType::RightArm);
    const auto ItemFromBack = RemoveItemFromEquipment(EEquipmentType::Back);

    TryAddItemToEquipment(ItemFromRightArm, EEquipmentType::Back);
    TryAddItemToEquipment(ItemFromBack, EEquipmentType::RightArm);

    bIsPlayingMontage = false;
    GetWorld()->GetTimerManager().ClearTimer(PlayerActionTimer);
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

    int32 SearchingElementIndex{-1};
    if (PersonalSlots.Find(Item, SearchingElementIndex))
    {
        OutSlotIndex = SearchingElementIndex;
        OutAmount = PersonalSlots[OutSlotIndex].Amount;
        return true;
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
    FVector Start = ViewLocation + (SpringArmComponent->TargetArmLength + 50.f) * VeiwRotation.Vector();
    FVector End = ViewLocation + (SpringArmComponent->TargetArmLength + 300.f) * VeiwRotation.Vector();

    FHitResult HitResult;
    // UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, 20.f, ETraceTypeQuery::TraceTypeQuery1, false, {this}, EDrawDebugTrace::ForDuration, HitResult, true);
    GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);

    if (HitResult.bBlockingHit)
    {
        UE_LOG(AwesomeCharacter, Display, TEXT("%s"), *HitResult.GetActor()->GetName());
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

    OnSlotChanged_OnClient(Slots[Index], Index, ESlotLocationType::PersonalSlots);
    return true;
}

void AAwesomeBaseCharacter::EquipItem(UClass* Class, UStaticMesh* NewMesh, FName SocketName, EEquipmentType Type)
{
    /* handle on server */
    if (!GetWorld()) return;
    auto PlayerEquippedItem = GetWorld()->SpawnActor<AAwesomeEquipmentActor>(Class);
    if (!PlayerEquippedItem) return;
    SetStaticMesh_Multicast(PlayerEquippedItem, NewMesh);
    FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
    PlayerEquippedItem->AttachToComponent(GetMesh(), AttachmentRules, SocketName);
    EquippedItemsMap.Add(Type, PlayerEquippedItem);
}

bool AAwesomeBaseCharacter::TrySellItem(const FSlot& SellingItem)
{
    /* handle on server */
    if (!ActiveShop) return false;

    const auto ItemDataPtr = SellingItem.DataTableRowHandle.GetRow<FItemData>("");
    if (!ItemDataPtr) return false;
    Money += ItemDataPtr->Cost;

    OnMoneyChanged_OnClient(Money);

    return ActiveShop->SellItem(SellingItem);
}

void AAwesomeBaseCharacter::BuyItem(const FSlot& BuyingItem, uint8 Index)
{
    /* handle on server */
    if (!ActiveShop) return;

    const auto ItemDataPtr = BuyingItem.DataTableRowHandle.GetRow<FItemData>("");
    if (!ItemDataPtr) return;
    if (ItemDataPtr->Cost > Money) return;
    Money -= ItemDataPtr->Cost;

    OnMoneyChanged_OnClient(Money);

    ActiveShop->BuyItem(Index);
}

void AAwesomeBaseCharacter::OnHUDWidgetSwitch(ESlateVisibility Visibility)
{
    if (Visibility == ESlateVisibility::Hidden)
    {
        SwitchMovement_OnServer(true);
        if (ActiveShop)
        {
            StopTrading_OnServer();
            OnTrading_OnClient(false);
        }
    }
    if (Visibility == ESlateVisibility::Visible)
    {
        SwitchMovement_OnServer(false);
    }
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

void AAwesomeBaseCharacter::PlayAnimMontage_Multicast_Implementation(UAnimMontage* Montage, float PlayRate)
{
    PlayAnimMontage(Montage, PlayRate);
}

void AAwesomeBaseCharacter::OnTrading_OnClient_Implementation(bool Enabled)
{
    OnTrading.Broadcast(Enabled);
}

void AAwesomeBaseCharacter::OpenInventory_OnClient_Implementation()
{
    const auto AwesomeController = Cast<AAwesomePlayerController>(Controller);
    if (AwesomeController)
    {
        AwesomeController->OpenInventory();
    }
}

void AAwesomeBaseCharacter::StopTrading_OnServer_Implementation()
{
    if (!ActiveShop) return;
    ActiveShop->StopTrading(this);
    ActiveShop = nullptr;
}

// void AAwesomeBaseCharacter::MoneyChanged_OnRep()
//{
//     UE_LOG(AwesomeCharacter, Display, TEXT("Money: %i"), Money);
//     OnMoneyValueChanged.Broadcast(Money);
// }

void AAwesomeBaseCharacter::OnMoneyChanged_OnClient_Implementation(int32 Value)
{
    OnMoneyValueChanged.Broadcast(Value);
}

void AAwesomeBaseCharacter::SwitchMovement_OnServer_Implementation(bool bCanMove)
{
    bCanMove ? GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking) : GetCharacterMovement()->DisableMovement();
}