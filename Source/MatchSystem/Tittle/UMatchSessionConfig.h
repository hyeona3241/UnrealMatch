// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OnlineSessionSettings.h"
#include "UMatchSessionConfig.generated.h"

/**
 *  나중에 매치 세션 설정 형태로 만들 수도 있을듯
 *  인원수 제한 매치를 만든다거나
 */
UCLASS()
class MATCHSYSTEM_API UUMatchSessionConfig : public UObject
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    bool bIsLANMatch = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    int32 NumPublicConnections = 4;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    FString MatchType = TEXT("FreeForAll");

    // 세션 설정을 만들어주는 헬퍼
    void FillSettings(FOnlineSessionSettings& OutSettings) const
    {
        OutSettings.bIsLANMatch = bIsLANMatch;
        OutSettings.NumPublicConnections = NumPublicConnections;
        OutSettings.bAllowJoinInProgress = true;
        OutSettings.bAllowJoinViaPresence = true;
        OutSettings.bShouldAdvertise = true;
        OutSettings.bUsesPresence = true;
        OutSettings.bUseLobbiesIfAvailable = true;
        OutSettings.Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
    }
	
};
