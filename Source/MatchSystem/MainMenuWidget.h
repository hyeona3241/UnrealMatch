// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class MATCHSYSTEM_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

    // 버튼 바인딩용
    UPROPERTY(meta = (BindWidget))
    class UButton* Btn_CreateRoom;

    UPROPERTY(meta = (BindWidget))
    class UButton* Btn_JoinRoom;

    UPROPERTY(meta = (BindWidget))
    class UButton* Btn_ConfirmCreate;

    UPROPERTY(meta = (BindWidget))
    class UButton* Btn_Map1;

    UPROPERTY(meta = (BindWidget))
    class UButton* Btn_Map2;

    UPROPERTY(meta = (BindWidget))
    class UButton* Btn_Map3;

    UPROPERTY(meta = (BindWidget))
    class UBorder* Border_Map1;

    UPROPERTY(meta = (BindWidget))
    class UBorder* Border_Map2;

    UPROPERTY(meta = (BindWidget))
    class UBorder* Border_Map3;

    UPROPERTY(meta = (BindWidget))
    class UWidget* MapSelectionPanel;

    // 내부 상태 저장용
    FName SelectedMap;

private:
    UFUNCTION()
    void OnClickCreateRoom();

    UFUNCTION()
    void OnClickJoinRoom();

    UFUNCTION()
    void OnClickConfirmCreate();

    UFUNCTION()
    void SetSelectedMap(FName MapName);

    UFUNCTION()
    void OnClickMap1();

    UFUNCTION()
    void OnClickMap2();

    UFUNCTION()
    void OnClickMap3();
};
