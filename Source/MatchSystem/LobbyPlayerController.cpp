// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerController.h"
#include <Kismet/GameplayStatics.h>
#include "Camera/CameraActor.h"
#include "LobbyGameMode.h"

void ALobbyPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocalController() && LobbyWidgetClass)
    {
        // 카메라 고정 딜레이 적용
        FTimerHandle TimerHandle;
        GetWorldTimerManager().SetTimer(TimerHandle, this, &ThisClass::SetLobbyCameraView, 0.1f, false);

        // 위젯 생성 및 표시
        MatchLobbyWidget = CreateWidget<UMatchLobbyWidget>(this, LobbyWidgetClass);
        if (MatchLobbyWidget)
        {
            MatchLobbyWidget->AddToViewport();

            // 마우스 커서 설정
            bShowMouseCursor = true;
            FInputModeUIOnly InputMode;
            InputMode.SetWidgetToFocus(MatchLobbyWidget->TakeWidget());
            SetInputMode(InputMode);

            Server_RequestUpdateSlots();
        }
    }
}


void ALobbyPlayerController::SetLobbyCameraView()
{
    UWorld* World = GetWorld();
    if (!World) return;

    TArray<AActor*> FoundCameras;
    UGameplayStatics::GetAllActorsOfClass(World, ACameraActor::StaticClass(), FoundCameras);

    for (AActor* Actor : FoundCameras)
    {
        if (Actor && Actor->GetName().Contains(TEXT("BP_LobbyCamera")))
        {
            SetViewTargetWithBlend(Actor, 0.5f); // 부드럽게 전환
            UE_LOG(LogTemp, Warning, TEXT("LobbyCamera found and set as view target."));
            return;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("LobbyCamera not found!"));
}


void ALobbyPlayerController::Client_UpdatePlayerSlots_Implementation()
{
    UE_LOG(LogTemp, Warning, TEXT("Client_UpdatePlayerSlots called on %s"), *GetName());

    if (MatchLobbyWidget)
    {
        MatchLobbyWidget->UpdatePlayerSlots();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("MatchLobbyWidget is null in Client_UpdatePlayerSlots"));
    }
}

void ALobbyPlayerController::Server_RequestUpdateSlots_Implementation()
{
    if (ALobbyGameMode* GM = GetWorld()->GetAuthGameMode<ALobbyGameMode>())
    {
        GM->BroadcastUpdatePlayerSlots();
    }
}
