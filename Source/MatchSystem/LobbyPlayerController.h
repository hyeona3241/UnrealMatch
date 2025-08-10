// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MatchLobbyWidget.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MATCHSYSTEM_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> LobbyWidgetClass;

	UPROPERTY()
	UMatchLobbyWidget* LobbyWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	ACameraActor* LobbyCameraRef;


	UPROPERTY()
	UMatchLobbyWidget* MatchLobbyWidget;

public:
	// Client_UpdatePlayerSlots_Implementation 자동 호출
	UFUNCTION(Client, Reliable)
	void Client_UpdatePlayerSlots();

	void Client_UpdatePlayerSlots_Implementation();

	UFUNCTION(Server, Reliable)
	void Server_RequestUpdateSlots();

private:
	void SetLobbyCameraView();

	

	
};
