// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MATCHSYSTEM_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	
public:
    ALobbyGameMode();

    virtual void PostLogin(APlayerController* NewPlayer) override;
    virtual void Logout(AController* Exiting) override;

protected:
    //서버용
    virtual void BeginPlay() override;

    // 로비 자리에 해당하는 위치 정보
    UPROPERTY(EditDefaultsOnly, Category = "Lobby")
    TArray<AActor*> LobbySlots; // TargetPoint들 미리 배치

    // 현재 접속한 플레이어들 (순서 중요)
    UPROPERTY()
    TArray<APlayerController*> PlayerQueue;

    // 플레이어 자리 배치
    void AssignPlayerToSlot(APlayerController* Player);

    // 자리 재정렬
    void ReorderSlots();

public:
    void BroadcastUpdatePlayerSlots();
	
};
