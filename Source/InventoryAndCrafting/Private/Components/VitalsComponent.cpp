// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/VitalsComponent.h"
#include "Net/UnrealNetwork.h"

UVitalsComponent::UVitalsComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UVitalsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UVitalsComponent, Health);
    DOREPLIFETIME(UVitalsComponent, Thirst);
    DOREPLIFETIME(UVitalsComponent, Hunger);
    DOREPLIFETIME(UVitalsComponent, MaxHealth);
    DOREPLIFETIME(UVitalsComponent, MaxThirst);
    DOREPLIFETIME(UVitalsComponent, MaxHunger);
}

void UVitalsComponent::ChangeVitalParameter_OnServer_Implementation(EVitalParameterType Type, float Amount)
{
    float ParamValueToUpdate{0.f}, MaxParamValue{0.f};
    switch (Type)
    {
        case EVitalParameterType::Thirst:
            Thirst = FMath::Clamp(Thirst + Amount, 0.f, MaxThirst);
            ParamValueToUpdate = Thirst;
            MaxParamValue = MaxThirst;
            break;
        case EVitalParameterType::Hunger:
            Hunger = FMath::Clamp(Hunger + Amount, 0.f, MaxHunger);
            ParamValueToUpdate = Hunger;
            MaxParamValue = MaxHunger;
            break;
        case EVitalParameterType::Health:
            Health = FMath::Clamp(Health + Amount, 0.f, MaxHealth);
            ParamValueToUpdate = Health;
            MaxParamValue = MaxHealth;
            break;
        case EVitalParameterType::MaxHealth:
            MaxHealth = FMath::Clamp(MaxHealth + Amount, 0.f, MAX_FLT);
            ParamValueToUpdate = Health;
            MaxParamValue = MaxHealth;
            break;
        case EVitalParameterType::NotVitalParameter: return;
        default: return;
    }

    UpdateVitalParameterWidget_OnClient(Type, ParamValueToUpdate, MaxParamValue);
}

void UVitalsComponent::BeginPlay()
{
    Super::BeginPlay();

    if (GetOwner()->HasAuthority())
    {
        MaxHealth = 100.f;
        MaxThirst = 100.f;
        MaxHunger = 100.f;
        Health = MaxHealth;
        Thirst = 0.f;
        Hunger = 0.f;

        float ReduceRate{0.5f};
        GetWorld()->GetTimerManager().SetTimer(ReduceVitalsTimer, this, &UVitalsComponent::ReduceVitals, ReduceRate, true);
    }

    UpdateVitalParameterWidget_OnClient(EVitalParameterType::Health, Health, MaxHealth);
    UpdateVitalParameterWidget_OnClient(EVitalParameterType::Thirst, Thirst, MaxThirst);
    UpdateVitalParameterWidget_OnClient(EVitalParameterType::Hunger, Hunger, MaxHunger);
}

void UVitalsComponent::UpdateVitalParameterWidget_OnClient_Implementation(EVitalParameterType Type, float Param, float MaxValue)
{
    float Persentage = MaxValue ? Param / MaxValue : 0.f;
    OnVitalParameterChanged.Broadcast(Type, Persentage);
}

void UVitalsComponent::ReduceVitals()
{
    float HungerAmount{1.f};
    ChangeVitalParameter_OnServer(EVitalParameterType::Hunger, HungerAmount);

    float ThirstAmount{1.f};
    ChangeVitalParameter_OnServer(EVitalParameterType::Thirst, ThirstAmount);

    if (FMath::IsNearlyEqual(Hunger, MaxHunger) || FMath::IsNearlyEqual(Thirst, MaxThirst))
    {
        float DrainHealthAmount{-1.f};
        ChangeVitalParameter_OnServer(EVitalParameterType::Health, DrainHealthAmount);
    }
}