#include "PlayerSlotWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/CheckBox.h"
//#include "LobbyPlayerState.h"

void UPlayerSlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 기본 상태는 슬롯이 비어있는 상태
    SetEmptyState();
}

void UPlayerSlotWidget::SetPlayerInfo(const FString& PlayerName, bool bReady)
{
    if (InviteButton) InviteButton->SetVisibility(ESlateVisibility::Collapsed);

    if (NameText)
    {
        NameText->SetText(FText::FromString(PlayerName));
        NameText->SetVisibility(ESlateVisibility::Visible);
    }

    if (NameWiget)
    {
        NameWiget->SetVisibility(ESlateVisibility::Visible);
    }

    if (ReadyCheckBox)
    {
        ReadyCheckBox->SetIsChecked(bReady);
        ReadyCheckBox->SetVisibility(ESlateVisibility::Visible);
    }

    bIsAssigned = true;
}


void UPlayerSlotWidget::ClearSlot()
{
    if (NameText) NameText->SetText(FText::FromString(TEXT("")));
    if (NameWiget) NameWiget->SetVisibility(ESlateVisibility::Collapsed);
    if (ReadyCheckBox)
    {
        ReadyCheckBox->SetIsChecked(false);
        ReadyCheckBox->SetVisibility(ESlateVisibility::Collapsed);
    }
    if (InviteButton)
        InviteButton->SetVisibility(ESlateVisibility::Visible);

    bIsAssigned = false;
}

void UPlayerSlotWidget::SetEmptyState()
{
    if (InviteButton) InviteButton->SetVisibility(ESlateVisibility::Visible);
    if (NameText) NameText->SetVisibility(ESlateVisibility::Collapsed);
    if (NameWiget) NameWiget->SetVisibility(ESlateVisibility::Collapsed);
    if (ReadyCheckBox)
    {
        ReadyCheckBox->SetIsChecked(false); // 준비 상태 false
        ReadyCheckBox->SetVisibility(ESlateVisibility::Collapsed);
    }

    bIsAssigned = false;
}
