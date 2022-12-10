// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AwesomeTypes.h"
#include "AwesomeBaseCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemPickedup, const TArray<FSlot>&);

UCLASS()
class INVENTORYANDCRAFTING_API AAwesomeBaseCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AAwesomeBaseCharacter();

    FOnItemPickedup OnItemPickedup;

    TArray<FSlot> GetSlots() const { return Slots; };

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    USpringArmComponent* SpringArmComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UCameraComponent* CameraComponent;

    virtual void BeginPlay() override;

public:
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
    TArray<FSlot> Slots;

    void MoveForward(float Amount);
    void MoveRight(float Amount);

    void TakeItem();

    void AddItem(const FSlot& Item);
};
