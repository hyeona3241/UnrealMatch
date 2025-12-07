// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "LobbyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MATCHSYSTEM_API ALobbyPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
    ALobbyPlayerState();

    /** 서버가 클라이언트에 자리 인덱스를 지정 (서버)*/
    void SetSlotIndex(int32 NewIndex);

    /** 클라이언트가 자신의 자리 인덱스를 알아내기 위한 Getter */
    FORCEINLINE int32 GetSlotIndex() const { return SlotIndex; }

protected:
    /** 복제되는 슬롯 인덱스 */
    UPROPERTY(ReplicatedUsing = OnRep_SlotIndex)
    int32 SlotIndex;

    /** RepNotify 함수 (서버->클라)*/
    UFUNCTION()
    void OnRep_SlotIndex();

    /** Replication 설정 (서버)*/
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:

    /** 클라이언트가 준비 상태를 서버에 전달하는 RPC */
    UFUNCTION(Server, Reliable)
    void Server_SetReady(bool bNewReady);

    /** 클라이언트에서 Ready 상태 변경 시 호출되는 RepNotify */
    UFUNCTION()
    void OnRep_Ready();

    /** 준비 상태 Getter */
    UFUNCTION(BlueprintCallable)
    bool IsReady() const { return bIsReady; }

protected:

    /** 준비 여부 변수 (서버 -> 클라 복제) */
    UPROPERTY(ReplicatedUsing = OnRep_Ready)
    bool bIsReady;

};
