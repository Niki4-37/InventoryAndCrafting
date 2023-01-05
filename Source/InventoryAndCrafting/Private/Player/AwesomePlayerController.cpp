// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/AwesomePlayerController.h"
#include "UI/AwesomeHUDWidget.h"

void AAwesomePlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocalPlayerController())
    {
        HUDWidget = CreateWidget<UAwesomeHUDWidget>(this, HUDWidgetClass);
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

    InputComponent->BindAction("OpenInventory", IE_Pressed, this, &AAwesomePlayerController::OpenInventory);
}

void AAwesomePlayerController::OpenInventory()
{
    if (!IsLocalPlayerController() || !HUDWidget) return;
    switch (HUDWidget->GetVisibility())
    {

        case (ESlateVisibility::Hidden):
        {
            HUDWidget->SetVisibility(ESlateVisibility::Visible);
            SetInputMode(FInputModeGameAndUI());
            bShowMouseCursor = true;
            break;
        }
        case (ESlateVisibility::Visible):
        {
            HUDWidget->SetVisibility(ESlateVisibility::Hidden);
            SetInputMode(FInputModeGameOnly());
            bShowMouseCursor = false;
            break;
        }
    }
    // OnHUDWidgetSwitch.Broadcast(HUDWidget->GetVisibility());
}
