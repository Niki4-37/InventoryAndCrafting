// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AwesomeTypes.h"
#include "VitalsComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class INVENTORYANDCRAFTING_API UVitalsComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UVitalsComponent();

    FOnVitalParameterChangedSignature OnVitalParameterChanged;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(Server, reliable)
    void ChangeVitalParameter_OnServer(EVitalParameterType Type, float Amount);

protected:
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
    float MaxHealth;

    virtual void BeginPlay() override;

private:
    UPROPERTY(Replicated)
    float Health;

    UPROPERTY(Replicated)
    float Thirst;

    UPROPERTY(Replicated)
    float Hunger;

    UPROPERTY(Replicated)
    float MaxThirst{100.f};

    UPROPERTY(Replicated)
    float MaxHunger{100.f};

    FTimerHandle ReduceVitalsTimer;

    UFUNCTION(Client, unreliable)
    void UpdateVitalParameterWidget_OnClient(EVitalParameterType Type, float Param, float MaxValue);

    void ReduceVitals();
};
