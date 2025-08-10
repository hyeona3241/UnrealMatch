// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerSlotWidget.h"
#include "MatchLobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class MATCHSYSTEM_API UMatchLobbyWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

    // 연결될 위젯 바인딩
    UPROPERTY(meta = (BindWidget))
    class UButton* Btn_Back;

    UPROPERTY(meta = (BindWidget))
    class UButton* Btn_PlayerList;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Text_Currency;

    UPROPERTY(meta = (BindWidget))
    class UWidget* Panel_BendingItems;

    UPROPERTY(meta = (BindWidget))
    class UWidget* Panel_CharacterSlots;

    UPROPERTY(meta = (BindWidget))
    class UButton* Btn_StartGame;

    UPROPERTY(meta = (BindWidget))
    class UButton* Btn_Customize;

    UPROPERTY(meta = (BindWidget))
    class UButton* Btn_Settings;
	
    UFUNCTION()
    void HandleBackButton();
	
public:
    /** 슬롯 갱신 함수 - 서버/클라 공용 */
    void UpdatePlayerSlots();

protected:

    /** 슬롯 4개 - 디자이너에서 Bind */
    UPROPERTY(meta = (BindWidget))
    UPlayerSlotWidget* PlayerSlot_0;

    UPROPERTY(meta = (BindWidget))
    UPlayerSlotWidget* PlayerSlot_1;

    UPROPERTY(meta = (BindWidget))
    UPlayerSlotWidget* PlayerSlot_2;

    UPROPERTY(meta = (BindWidget))
    UPlayerSlotWidget* PlayerSlot_3;
};
