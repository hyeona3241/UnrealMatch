#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/Widget.h"
#include "Kismet/GameplayStatics.h"
#include "MatchSystemCharacter.h"
#include "Components/Border.h"

void UMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    //if (Btn_CreateRoom) Btn_CreateRoom->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickCreateRoom);
    //if (Btn_JoinRoom) Btn_JoinRoom->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickJoinRoom);
    //if (Btn_ConfirmCreate) Btn_ConfirmCreate->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickConfirmCreate);
    //if (Btn_Map1) Btn_Map1->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickMap1);
    //if (Btn_Map2) Btn_Map2->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickMap2);
    //if (Btn_Map3) Btn_Map3->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickMap3);

    if (MapSelectionPanel)
    {
        MapSelectionPanel->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UMainMenuWidget::OnClickCreateRoom()
{
    if (MapSelectionPanel)
        MapSelectionPanel->SetVisibility(ESlateVisibility::Visible);
}

// 참가랑 생성 실패시 위젯 다시 뜨도록 코드 추가하기


void UMainMenuWidget::OnClickJoinRoom()
{
    UE_LOG(LogTemp, Warning, TEXT("JoinRoom button clicked"));

    if (auto* Char = Cast<AMatchSystemCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
    {
        UE_LOG(LogTemp, Warning, TEXT("Calling JoinGameSession on character %s"), *Char->GetName());
        Char->JoinGameSession();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to cast to AMatchSystemCharacter"));
    }
}

void UMainMenuWidget::OnClickConfirmCreate()
{
    if (auto* Char = Cast<AMatchSystemCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
    {
        Char->SelectedMap = SelectedMap;
        Char->CreateGameSession();
    }
}

void UMainMenuWidget::SetSelectedMap(FName MapName)
{
    SelectedMap = MapName;

    if (Border_Map1) Border_Map1->SetBrushColor(FLinearColor::White);
    if (Border_Map2) Border_Map2->SetBrushColor(FLinearColor::White);
    if (Border_Map3) Border_Map3->SetBrushColor(FLinearColor::White);

    if (MapName == "Map1" && Border_Map1)
        Border_Map1->SetBrushColor(FLinearColor::Yellow);
    else if (MapName == "Map2" && Border_Map2)
        Border_Map2->SetBrushColor(FLinearColor::Yellow);
    else if (MapName == "Map3" && Border_Map3)
        Border_Map3->SetBrushColor(FLinearColor::Yellow);
}

void UMainMenuWidget::OnClickMap1() { SetSelectedMap("Map1"); }
void UMainMenuWidget::OnClickMap2() { SetSelectedMap("Map2"); }
void UMainMenuWidget::OnClickMap3() { SetSelectedMap("Map3"); }
