// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/AwesomePlayerController.h"
#include "UI/PlayerHUDWidget.h"
#include "Components/InventoryComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PawnMovementComponent.h"

void AAwesomePlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocalPlayerController())
    {
        HUDWidget = CreateWidget<UPlayerHUDWidget>(this, HUDWidgetClass);
        if (HUDWidget)
        {
            HUDWidget->AddToViewport();
            HUDWidget->ShowInventory(false);
        }
    }
}

void AAwesomePlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    if (!InputComponent) return;

    InputComponent->BindAction("OpenCloseInventory", IE_Pressed, this, &AAwesomePlayerController::OpenCloseInventory);
}

void AAwesomePlayerController::OnPossess(APawn* aPawn)
{
    Super::OnPossess(aPawn);
}

void AAwesomePlayerController::OpenInventory()
{
    if (!IsLocalPlayerController() || !HUDWidget) return;
    HUDWidget->ShowInventory(true);
    SetInputMode(FInputModeGameAndUI());
    bShowMouseCursor = true;

    GetPawn()->DisableInput(this);
}

void AAwesomePlayerController::CloseInventory()
{
    if (!IsLocalPlayerController() || !HUDWidget) return;
    HUDWidget->ShowInventory(false);
    SetInputMode(FInputModeGameOnly());
    bShowMouseCursor = false;

    GetPawn()->EnableInput(this);

    const auto InventoryComponent = GetPawn()->FindComponentByClass<UInventoryComponent>();
    if (InventoryComponent)
    {
        InventoryComponent->StopTrading_OnServer();
    }
}

void AAwesomePlayerController::OpenCloseInventory()
{
    HUDWidget->GetInventoryVisibility()  //
        ?
        CloseInventory()  //
        :
        OpenInventory();
}