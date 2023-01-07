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
            HUDWidget->SetVisibility(ESlateVisibility::Hidden);
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
    HUDWidget->SetVisibility(ESlateVisibility::Visible);
    SetInputMode(FInputModeGameAndUI());
    bShowMouseCursor = true;

    GetPawn()->DisableInput(this);
}

void AAwesomePlayerController::CloseInventory()
{
    if (!IsLocalPlayerController() || !HUDWidget) return;
    HUDWidget->SetVisibility(ESlateVisibility::Hidden);
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
    switch (HUDWidget->GetVisibility())
    {
        case (ESlateVisibility::Hidden): OpenInventory(); break;
        case (ESlateVisibility::Visible): CloseInventory(); break;
    }
}