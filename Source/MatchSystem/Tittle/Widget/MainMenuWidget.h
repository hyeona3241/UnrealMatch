// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Widget.h"
#include "Components/Border.h"
#include "MainMenuWidget.generated.h"

/**
 * 
 */

UCLASS()
class MATCHSYSTEM_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category = "MainMenu|Buttons")
    void OnClickCreateRoom();

    UFUNCTION(BlueprintCallable, Category = "MainMenu|Buttons")
    void OnClickJoinRoom();

    UFUNCTION(BlueprintCallable, Category = "MainMenu|Buttons")
    void OnClickConfirmCreate();

    UFUNCTION(BlueprintCallable, Category = "MainMenu|Buttons")
    void OnClickMap1();

    UFUNCTION(BlueprintCallable, Category = "MainMenu|Buttons")
    void OnClickMap2();

    UFUNCTION(BlueprintCallable, Category = "MainMenu|Buttons")
    void OnClickMap3();

protected:
    virtual void NativeConstruct() override;

    // 맵 선택 패널 (보이기/숨기기)
    UPROPERTY(meta = (BindWidget))
    UWidget* MapSelectionPanel;

    // 맵 선택 보더들 (색 변경)
    UPROPERTY(meta = (BindWidget))
    UBorder* Border_Map1;

    UPROPERTY(meta = (BindWidget))
    UBorder* Border_Map2;

    UPROPERTY(meta = (BindWidget))
    UBorder* Border_Map3;

    UPROPERTY(BlueprintReadOnly, Category = "MainMenu")
    FName SelectedMap;

private:
    UFUNCTION()
    void SetSelectedMap(FName MapName);
};
