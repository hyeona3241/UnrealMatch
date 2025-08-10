// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class MATCHSYSTEM_API UPlayerSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    void NativeConstruct();

    void SetPlayerInfo(const FString& PlayerName, bool bReady);
    void ClearSlot();

    void SetSlotAssigned(bool bAssigned) { bIsAssigned = bAssigned; }
    bool IsSlotAssigned() const { return bIsAssigned; }

    void SetEmptyState();

protected:
    UPROPERTY(meta = (BindWidget))
    class UWidget* NameWiget;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* NameText;

    UPROPERTY(meta = (BindWidget))
    class UCheckBox* ReadyCheckBox;

    UPROPERTY(meta = (BindWidget))
    class UButton* InviteButton;

private:
    bool bIsAssigned = false;
	
};
