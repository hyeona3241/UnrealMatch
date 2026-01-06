// MatchLobbyWidget.cpp

#include "MatchLobbyWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/HorizontalBox.h"
#include "MatchSystem/Change/Lobby/LobbyPlayerState.h"

void UMatchLobbyWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (Btn_Back)
        Btn_Back->OnClicked.AddDynamic(this, &UMatchLobbyWidget::HandleBackButton);

    // 나머지 버튼들에 대해서도 마찬가지로 바인딩
}

void UMatchLobbyWidget::HandleBackButton()
{
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        PC->ClientTravel(TEXT("/Game/ThirdPerson/Maps/ThirdPersonMap"), ETravelType::TRAVEL_Absolute);
    }
}

void UMatchLobbyWidget::UpdatePlayerSlots()
{
    TArray<UPlayerSlotWidget*> Slots = { PlayerSlot_0, PlayerSlot_1, PlayerSlot_2, PlayerSlot_3 };

    for (UPlayerSlotWidget* slot : Slots)
        slot->SetEmptyState(); // 먼저 전부 비운다

    UWorld* World = GetWorld();
    if (!World) return;

    int32 Index = 0;
    for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It && Index < Slots.Num(); ++It)
    {
        if (APlayerController* PC = It->Get())
        {
            if (ALobbyPlayerState* PS = PC->GetPlayerState<ALobbyPlayerState>())
            {
                FString Name = PS->GetPlayerName();
                bool bReady = PS->IsReady();


                if (GEngine)
                {
                    GEngine->AddOnScreenDebugMessage(
                        -1,
                        15.f,
                        FColor::Green,
                        FString::Printf(TEXT("Slot[%d] Assigned to %s"), Index, *Name)
                    );
                }

                Slots[Index]->SetPlayerInfo(Name, bReady);
                ++Index;
            }
        }
    }
}

