// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/AwesomeBaseCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/VitalsComponent.h"
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

    InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");

    VitalsComponent = CreateDefaultSubobject<UVitalsComponent>("VitalsComponent");
}

void AAwesomeBaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        bIsPlayingMontage = false;
    }
}

void AAwesomeBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AAwesomeBaseCharacter, bIsPlayingMontage);
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

void AAwesomeBaseCharacter::SwapWeapons_OnServer_Implementation()
{
    SwapItems(SwapWeaponsMontage, EEquipmentType::RightArm, EEquipmentType::LeftArm);
}

void AAwesomeBaseCharacter::DrawWeapon_OnServer_Implementation()
{
    SwapItems(DrawWeaponMontage, EEquipmentType::RightArm, EEquipmentType::Back);
}

void AAwesomeBaseCharacter::SwapItems(UAnimMontage* Montage, EEquipmentType FirstSlotType, EEquipmentType SecondSlotType)
{
    /* handled on server */
    bool bCanPlayMontage = InventoryComponent && InventoryComponent->HasEquipmentToSwap(FirstSlotType, SecondSlotType) && !bIsPlayingMontage && GetWorld();
    if (!bCanPlayMontage) return;
    bIsPlayingMontage = true;
    float MontagePlayRate = 2.f;
    float HalfDuration = Montage ? Montage->CalculateSequenceLength() / (2 * MontagePlayRate) : 0.f;
    PlayAnimMontage_Multicast(Montage, MontagePlayRate);

    FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(InventoryComponent, &UInventoryComponent::SwapItems, FirstSlotType, SecondSlotType);
    GetWorld()->GetTimerManager().SetTimer(MontageTimer, TimerDelegate, HalfDuration, false);
    GetWorld()->GetTimerManager().SetTimer(ResetTimer, this, &AAwesomeBaseCharacter::ResetMontageTimer, HalfDuration * 2);
}

void AAwesomeBaseCharacter::ResetMontageTimer()
{
    bIsPlayingMontage = false;
    GetWorld()->GetTimerManager().ClearTimer(MontageTimer);
    GetWorld()->GetTimerManager().ClearTimer(ResetTimer);
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
    FVector Start = ViewLocation + (SpringArmComponent->TargetArmLength + 50.f) * VeiwRotation.Vector();
    FVector End = ViewLocation + (SpringArmComponent->TargetArmLength + 300.f) * VeiwRotation.Vector();

    FHitResult HitResult;
    // UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, 20.f, ETraceTypeQuery::TraceTypeQuery1, false, {this}, EDrawDebugTrace::ForDuration, HitResult, true);
    GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);

    if (HitResult.bBlockingHit)
    {
        UE_LOG(AwesomeCharacter, Display, TEXT("%s"), *HitResult.GetActor()->GetName());
        DrawDebugLine(GetWorld(), Start, HitResult.ImpactPoint, FColor::Blue, false, 5.f, 0U, 3.f);
        if (const auto ActorWithInterface = Cast<IInteractionInterface>(HitResult.GetActor()))
        {
            ActorWithInterface->Interact(this);
        }
    }
    else
    {
        DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 5.f, 0U, 3.f);
    }
}

void AAwesomeBaseCharacter::PlayAnimMontage_Multicast_Implementation(UAnimMontage* Montage, float PlayRate)
{
    PlayAnimMontage(Montage, PlayRate);
}

void AAwesomeBaseCharacter::SwitchMovement_OnServer_Implementation(bool bCanMove)
{
    bCanMove ? GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking) : GetCharacterMovement()->DisableMovement();
}