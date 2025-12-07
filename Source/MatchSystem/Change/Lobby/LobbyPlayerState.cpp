#include "LobbyPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

ALobbyPlayerState::ALobbyPlayerState()
{
    bReplicates = true;
    SlotIndex = -1;
    bIsReady = false;
}

void ALobbyPlayerState::SetSlotIndex(int32 NewIndex)
{
    if (HasAuthority())
    {
        SlotIndex = NewIndex;
        OnRep_SlotIndex(); // 서버에서도 직접 호출
    }
}

void ALobbyPlayerState::OnRep_SlotIndex()
{
    UE_LOG(LogTemp, Log, TEXT("[LobbyPlayerState] SlotIndex set to %d for %s"), SlotIndex, *GetPlayerName());

    /*친구 초대 버튼 비활성화
    닉네임 텍스트 GetPlayerName()으로 설정
    준비 체크박스 활성화*/
}

void ALobbyPlayerState::Server_SetReady_Implementation(bool bNewReady)
{
    bIsReady = bNewReady;
    OnRep_Ready(); // 서버에서도 로컬 UI 반영 가능
}

void ALobbyPlayerState::OnRep_Ready()
{
    UE_LOG(LogTemp, Log, TEXT("[LobbyPlayerState] %s Ready Status: %s"), *GetPlayerName(), bIsReady ? TEXT("Ready") : TEXT("Not Ready"));

    //체크박스 UI 상태 업데이트
}

void ALobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ALobbyPlayerState, SlotIndex);
    DOREPLIFETIME(ALobbyPlayerState, bIsReady);
}
