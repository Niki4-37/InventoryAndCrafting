// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AwesomeTypes.h"
#include "Interfaces/InteractionInterface.h"
#include "AwesomeBaseCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInventoryComponent;

UCLASS()
class INVENTORYANDCRAFTING_API AAwesomeBaseCharacter : public ACharacter, public IInteractionInterface
{
    GENERATED_BODY()

public:
    AAwesomeBaseCharacter();

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    USpringArmComponent* SpringArmComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UCameraComponent* CameraComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UInventoryComponent* InventoryComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UAnimMontage* SwapWeaponsMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UAnimMontage* DrawWeaponMontage;

    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    UPROPERTY(Replicated)
    bool bIsPlayingMontage{false};

    FTimerHandle MontageTimer;
    FTimerHandle ResetTimer;

    void MoveForward(float Amount);
    void MoveRight(float Amount);

    UFUNCTION(Server, Reliable)
    void SwapWeapons_OnServer();
    UFUNCTION(Server, Reliable)
    void DrawWeapon_OnServer();

    void SwapItems(UAnimMontage* Montage, EEquipmentType FirstSlotType, EEquipmentType SecondSlotType);
    void ResetMontageTimer();

    void TakeItem();

    UFUNCTION(NetMulticast, Unreliable)
    void PlayAnimMontage_Multicast(UAnimMontage* Montage, float PlayRate);

    UFUNCTION(Server, unreliable)
    void SwitchMovement_OnServer(bool bCanMove);
};